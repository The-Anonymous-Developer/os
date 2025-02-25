#ifndef GDT_H
#define GDT_H

#include <stdint.h>

// Cross-platform structure packing
#ifdef _MSC_VER
#define PACKED
#pragma pack(push, 1)
#else
#define PACKED __attribute__((packed))
#endif

#define GDT_ENTRIES 5

// GDT entry structure
typedef struct {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_middle;
    uint8_t access;
    uint8_t granularity;
    uint8_t base_high;
} PACKED gdt_entry_t;

// GDTR structure
typedef struct {
    uint16_t limit;
    uint64_t base;
} PACKED gdt_ptr_t;

void init_gdt(void);

#ifdef _MSC_VER
#pragma pack(pop)
#endif

#endif // GDT_H