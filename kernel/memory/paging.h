#ifndef PAGING_H
#define PAGING_H

#include "../include/kernel.h"

// Page table entry flags
#define PAGE_PRESENT    (1ULL << 0)
#define PAGE_WRITABLE   (1ULL << 1)
#define PAGE_USER       (1ULL << 2)
#define PAGE_HUGE       (1ULL << 7)

// Page table entry types
typedef uint64_t page_entry_t;
typedef page_entry_t pml4_entry_t;
typedef page_entry_t pdpt_entry_t;
typedef page_entry_t pd_entry_t;
typedef page_entry_t pt_entry_t;

// Function declarations
error_t paging_init(void);
error_t paging_map_page(void* virt_addr, void* phys_addr, uint64_t flags);
error_t paging_unmap_page(void* virt_addr);
void* paging_get_physical_address(void* virt_addr);

#endif // PAGING_H