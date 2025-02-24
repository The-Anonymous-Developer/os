#include "hardware.h"
#include "interrupt.h"

// System tick counter
volatile uint64_t system_ticks = 0;

// Interrupt service routine for timer (IRQ0)
void timer_handler(interrupt_frame_t* frame) {
    system_ticks++;
    
    // Send EOI (End of Interrupt) to PIC
    outb(0x20, 0x20);
}

// Generic interrupt handler
void interrupt_handler(interrupt_frame_t* frame) {
    switch (frame->interrupt_number) {
        case 32:  // Timer interrupt
            timer_handler(frame);
            break;
            
        default:
            // Handle unknown interrupt
            break;
    }
}