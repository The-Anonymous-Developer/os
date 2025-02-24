#include "hardware.h"
#include "interrupt.h"

#define IDT_ENTRIES 256

// Array of function pointers for interrupt stubs
typedef void (*interrupt_stub_t)(void);
extern interrupt_stub_t interrupt_stubs[];

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
        uint64_t handler = (uint64_t)interrupt_stubs[i];
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