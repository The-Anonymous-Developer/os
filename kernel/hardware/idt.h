#ifndef IDT_H
#define IDT_H

#include <stdint.h>
#include "interrupt.h"

// Cross-platform structure packing
#ifdef _MSC_VER
#define PACKED
#pragma pack(push, 1)
#else
#define PACKED __attribute__((packed))
#endif

#define IDT_ENTRIES 256

// IDT gate types
#define IDT_GATE_TASK     0x5
#define IDT_GATE_INT16    0x6
#define IDT_GATE_TRAP16   0x7
#define IDT_GATE_INT32    0xE
#define IDT_GATE_TRAP32   0xF

// IDT entry structure
typedef struct {
    uint16_t base_low;
    uint16_t selector;
    uint8_t ist;
    uint8_t flags;
    uint16_t base_middle;
    uint32_t base_high;
    uint32_t reserved;
} PACKED idt_entry_t;

// IDTR structure
typedef struct {
    uint16_t limit;
    uint64_t base;
} PACKED idt_ptr_t;

// Function declarations
void init_idt(void);
void set_idt_gate(uint8_t vector, uint64_t base, uint16_t sel, uint8_t flags);

#ifdef _MSC_VER
#pragma pack(pop)
#endif

#endif // IDT_H