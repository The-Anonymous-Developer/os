#include "hardware.h"
#include "interrupt.h"

// System tick counter for timer interrupts
volatile uint64_t system_ticks = 0;

// Exception messages for debugging
static const char* exception_messages[] = {
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Into Detected Overflow",
    "Out of Bounds",
    "Invalid Opcode",
    "No Coprocessor",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Bad TSS",
    "Segment Not Present",
    "Stack Fault",
    "General Protection Fault",
    "Page Fault",
    "Unknown Interrupt",
    "Coprocessor Fault",
    "Alignment Check",
    "Machine Check",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved"
};

// Timer interrupt handler
void timer_handler(interrupt_frame_t* frame) {
    system_ticks++;
    
    // Send EOI to PIC
    outb(0x20, 0x20);
}

void interrupt_handler(interrupt_frame_t* frame) {
    // Handle CPU exceptions (0-31)
    if (frame->interrupt_number < 32) {
        // Handle CPU exception
        // TODO: Add proper error handling and recovery
        for(;;); // Halt for now
    }
    // Handle timer interrupt (IRQ0 = 32)
    else if (frame->interrupt_number == 32) {
        timer_handler(frame);
    }
    // Handle keyboard interrupt (IRQ1 = 33)
    else if (frame->interrupt_number == 33) {
        // Read keyboard scan code
        uint8_t scancode = inb(0x60);
        
        // TODO: Add keyboard handling
        
        // Send EOI to PIC
        outb(0x20, 0x20);
    }
}

// Get current system ticks (for timing)
uint64_t get_system_ticks(void) {
    return system_ticks;
}