#include <stdint.h>
#include "../kernel.h"

void init_hardware(void) {
    // Disable interrupts using compiler-specific inline assembly
    #ifdef _MSC_VER
        __asm {
            cli
        }
    #else
        asm("cli");
    #endif

    // Basic hardware initialization
    // TODO: Initialize:
    // - GDT (Global Descriptor Table)
    // - IDT (Interrupt Descriptor Table)
    // - PIC (Programmable Interrupt Controller)
    // - PIT (Programmable Interval Timer)
}