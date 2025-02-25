#include "idt.h"
#include "hardware.h"
#include "interrupt.h"
#include "timer.h"
#include <stddef.h>

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

// Forward declaration of the helper function
static void set_idt_entry(uint8_t vector, uint64_t handler);

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
        set_idt_entry(i, handler);
    }

    // Set up timer interrupt (IRQ0)
    extern void timer_handler(interrupt_frame_t*);  // Declare external handler
    set_idt_entry(32, (uint64_t)timer_handler);

    // Load IDT
    #ifdef _MSC_VER
        __asm {
            lidt [idtp]
        }
    // #else
    //     __asm__ volatile (
    //         "lidt %0"
    //         : 
    //         : "m"(idtp)
    //         : "memory"
    //     );
    #endif

    // Initialize interrupt handlers
    init_interrupt_handlers();

    // Configure PIC
    pic_remap(IRQ_TIMER, IRQ_TIMER + 8);
    
    // Enable interrupts
    STI();
}

static void set_idt_entry(uint8_t vector, uint64_t handler) {
// Helper function to set IDT entries
    idt[vector].base_low = (uint16_t)(handler & 0xFFFF);
    idt[vector].base_middle = (uint16_t)((handler >> 16) & 0xFFFF);
    idt[vector].base_high = (uint32_t)((handler >> 32) & 0xFFFFFFFF);
    idt[vector].selector = 0x08;  // Kernel code segment
    idt[vector].ist = 0;
    idt[vector].flags = 0x8E;     // Present, Ring 0, Interrupt Gate
    idt[vector].reserved = 0;
}