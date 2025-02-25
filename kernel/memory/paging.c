#include "paging.h"
#include "heap.h"
#include "../hardware/cpu.h"
#include "../hardware/display.h"
#include "../include/kernel.h"

#define PAGE_SIZE 4096
#define ENTRIES_PER_TABLE 512
#define TABLE_MASK (~0xFFFULL)

static page_entry_t* pml4_table = NULL;

static void* allocate_zeroed_page(void) {
    void* page = heap_alloc(PAGE_SIZE);
    if (!page) {
        return NULL;
    }
    memset(page, 0, PAGE_SIZE);
    return page;
}

error_t paging_init(void) {
    if (pml4_table != NULL) {
        return ERR_INVALID_PARAM;  // Already initialized
    }

    pml4_table = allocate_zeroed_page();
    if (!pml4_table) {
        display_write("Failed to allocate PML4 table\n");
        return ERR_OUT_OF_MEMORY;
    }

    // Identity map first 2MB
    error_t status;
    for (uint64_t addr = 0; addr < 0x200000; addr += PAGE_SIZE) {
        status = paging_map_page((void*)addr, (void*)addr, 
                               PAGE_PRESENT | PAGE_WRITABLE);
        if (status != SUCCESS) {
            return status;
        }
    }

    write_cr3((uint64_t)pml4_table);
    return SUCCESS;
}

error_t paging_map_page(void* virt_addr, void* phys_addr, uint64_t flags) {
    if (!virt_addr || !phys_addr || !pml4_table) {
        return ERR_NULL_POINTER;
    }

    uint64_t addr = (uint64_t)virt_addr;
    uint64_t pml4_index = (addr >> 39) & 0x1FF;
    uint64_t pdpt_index = (addr >> 30) & 0x1FF;
    uint64_t pd_index = (addr >> 21) & 0x1FF;
    uint64_t pt_index = (addr >> 12) & 0x1FF;

    // Traverse/create PML4 entry
    pdpt_entry_t* pdpt;
    if (!(pml4_table[pml4_index] & PAGE_PRESENT)) {
        pdpt = allocate_zeroed_page();
        if (!pdpt) return ERR_OUT_OF_MEMORY;
        pml4_table[pml4_index] = ((uint64_t)pdpt) | flags;
    } else {
        pdpt = (pdpt_entry_t*)(pml4_table[pml4_index] & TABLE_MASK);
    }

    // Traverse/create PDPT entry
    pd_entry_t* pd;
    if (!(pdpt[pdpt_index] & PAGE_PRESENT)) {
        pd = allocate_zeroed_page();
        if (!pd) return ERR_OUT_OF_MEMORY;
        pdpt[pdpt_index] = ((uint64_t)pd) | flags;
    } else {
        pd = (pd_entry_t*)(pdpt[pdpt_index] & TABLE_MASK);
    }

    // Traverse/create PD entry
    pt_entry_t* pt;
    if (!(pd[pd_index] & PAGE_PRESENT)) {
        pt = allocate_zeroed_page();
        if (!pt) return ERR_OUT_OF_MEMORY;
        pd[pd_index] = ((uint64_t)pt) | flags;
    } else {
        pt = (pt_entry_t*)(pd[pd_index] & TABLE_MASK);
    }

    // Set page table entry
    pt[pt_index] = ((uint64_t)phys_addr) | flags;
    
    return SUCCESS;
}

void* paging_get_physical_address(void* virt_addr) {
    uint64_t addr = (uint64_t)virt_addr;
    uint64_t pml4_index = (addr >> 39) & 0x1FF;
    uint64_t pdpt_index = (addr >> 30) & 0x1FF;
    uint64_t pd_index = (addr >> 21) & 0x1FF;
    uint64_t pt_index = (addr >> 12) & 0x1FF;
    uint64_t page_offset = addr & 0xFFF;

    // Check if PML4 entry exists
    if (!(pml4_table[pml4_index] & PAGE_PRESENT)) {
        return NULL;
    }
    pdpt_entry_t* pdpt = (pdpt_entry_t*)(pml4_table[pml4_index] & ~0xFFF);

    // Check if PDPT entry exists
    if (!(pdpt[pdpt_index] & PAGE_PRESENT)) {
        return NULL;
    }
    pd_entry_t* pd = (pd_entry_t*)(pdpt[pdpt_index] & ~0xFFF);

    // Check if PD entry exists
    if (!(pd[pd_index] & PAGE_PRESENT)) {
        return NULL;
    }
    pt_entry_t* pt = (pt_entry_t*)(pd[pd_index] & ~0xFFF);

    // Check if PT entry exists
    if (!(pt[pt_index] & PAGE_PRESENT)) {
        return NULL;
    }

    // Get physical address from page table entry and add offset
    uint64_t phys_addr = (pt[pt_index] & ~0xFFF) + page_offset;
    return (void*)phys_addr;
}