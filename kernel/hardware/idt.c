#include "hardware.h"
#include "interrupt.h"
#include <stddef.h>  // For NULL definition

#define IDT_ENTRIES 256

// Declare all interrupt stubs (0-31 for CPU exceptions)
extern void interrupt_stub_0(void);
extern void interrupt_stub_1(void);
extern void interrupt_stub_2(void);
extern void interrupt_stub_3(void);
extern void interrupt_stub_4(void);
extern void interrupt_stub_5(void);
extern void interrupt_stub_6(void);
extern void interrupt_stub_7(void);
extern void interrupt_stub_8(void);
extern void interrupt_stub_9(void);
extern void interrupt_stub_10(void);
extern void interrupt_stub_11(void);
extern void interrupt_stub_12(void);
extern void interrupt_stub_13(void);
extern void interrupt_stub_14(void);
extern void interrupt_stub_15(void);
extern void interrupt_stub_16(void);
extern void interrupt_stub_17(void);
extern void interrupt_stub_18(void);
extern void interrupt_stub_19(void);
extern void interrupt_stub_20(void);
extern void interrupt_stub_21(void);
extern void interrupt_stub_22(void);
extern void interrupt_stub_23(void);
extern void interrupt_stub_24(void);
extern void interrupt_stub_25(void);
extern void interrupt_stub_26(void);
extern void interrupt_stub_27(void);
extern void interrupt_stub_28(void);
extern void interrupt_stub_29(void);
extern void interrupt_stub_30(void);
extern void interrupt_stub_31(void);

// Define complete array of handlers with fixed size
static void (*interrupt_handlers[32])(void) = {
    interrupt_stub_0,
    interrupt_stub_1,
    interrupt_stub_2,
    interrupt_stub_3,
    interrupt_stub_4,
    interrupt_stub_5,
    interrupt_stub_6,
    interrupt_stub_7,
    interrupt_stub_8,
    interrupt_stub_9,
    interrupt_stub_10,
    interrupt_stub_11,
    interrupt_stub_12,
    interrupt_stub_13,
    interrupt_stub_14,
    interrupt_stub_15,
    interrupt_stub_16,
    interrupt_stub_17,
    interrupt_stub_18,
    interrupt_stub_19,
    interrupt_stub_20,
    interrupt_stub_21,
    interrupt_stub_22,
    interrupt_stub_23,
    interrupt_stub_24,
    interrupt_stub_25,
    interrupt_stub_26,
    interrupt_stub_27,
    interrupt_stub_28,
    interrupt_stub_29,
    interrupt_stub_30,
    interrupt_stub_31
};

static idt_entry_t idt[IDT_ENTRIES];
static idt_ptr_t idtp;

void init_idt(void) {
    // Set up IDT pointer
    idtp.limit = (sizeof(idt_entry_t) * IDT_ENTRIES) - 1;
    idtp.base = (uint64_t)&idt;

    // Clear IDT
    for (int i = 0; i < IDT_ENTRIES; i++) {
        idt[i] = (idt_entry_t){0};
    }

    // Set up CPU exception handlers
    for (int i = 0; i < 32; i++) {
        uint64_t handler = (uint64_t)interrupt_handlers[i];
        idt[i].base_low = (uint16_t)(handler & 0xFFFF);
        idt[i].base_middle = (uint16_t)((handler >> 16) & 0xFFFF);
        idt[i].base_high = (uint32_t)((handler >> 32) & 0xFFFFFFFF);
        idt[i].selector = 0x08;  // Kernel code segment
        idt[i].ist = 0;
        idt[i].flags = 0x8E;     // Present, Ring 0, Interrupt Gate
        idt[i].reserved = 0;
    }

    // Set up timer interrupt (IRQ0)
    uint64_t timer = (uint64_t)timer_handler;
    idt[32].base_low = (uint16_t)(timer & 0xFFFF);
    idt[32].base_middle = (uint16_t)((timer >> 16) & 0xFFFF);
    idt[32].base_high = (uint32_t)((timer >> 32) & 0xFFFFFFFF);
    idt[32].selector = 0x08;  // Kernel code segment
    idt[32].ist = 0;
    idt[32].flags = 0x8E;     // Present, Ring 0, Interrupt Gate
    idt[32].reserved = 0;

    // Load IDT using cross-platform assembly
    #ifdef _MSC_VER
        ASM {
            lidt [idtp]
        }
    #else
        // ASM (
        //     "lidt %0"
        //     : 
        //     : "m"(idtp)
        //     : "memory"
        // );
    #endif
}