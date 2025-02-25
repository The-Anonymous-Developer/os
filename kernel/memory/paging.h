#ifndef PAGING_H
#define PAGING_H

#include <stdint.h>

// Page table entry flags
#define PAGE_PRESENT    (1 << 0)
#define PAGE_WRITABLE   (1 << 1)
#define PAGE_USER       (1 << 2)
#define PAGE_HUGE       (1 << 7)

// 4-level paging structures
typedef uint64_t pml4_entry_t;
typedef uint64_t pdpt_entry_t;
typedef uint64_t pd_entry_t;
typedef uint64_t pt_entry_t;

void paging_init(void);
void* paging_map_page(void* virt_addr, void* phys_addr, uint64_t flags);
void paging_unmap_page(void* virt_addr);
void* paging_get_physical_address(void* virt_addr);

#endif // PAGING_H