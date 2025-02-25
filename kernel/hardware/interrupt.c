#include "hardware.h"
#include "interrupt.h"
#include "ports.h"
#include "pic.h"
#include "display.h"
#include "../lib/string.h"
#include "idt.h"

// System tick counter for timer interrupts
static volatile uint64_t system_ticks = 0;

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
static void timer_handler(interrupt_frame_t* frame) {
    (void)frame;  // Silence unused parameter warning
    system_ticks++;
}

// Array of interrupt handlers
static interrupt_handler_t interrupt_handlers[256] = { 0 };

void register_interrupt_handler(uint8_t vector, interrupt_handler_t handler) {
    if (vector < 256) {
        interrupt_handlers[vector] = handler;
    }
}

void interrupt_handler(interrupt_frame_t* frame) {
    if (frame->interrupt_number < 256 && interrupt_handlers[frame->interrupt_number]) {
        interrupt_handlers[frame->interrupt_number](frame);
    } else if (frame->interrupt_number < 32) {
        // Handle CPU exceptions
        display_write("Exception: ");
        display_write(exception_messages[frame->interrupt_number]);
        display_write("\nError Code: ");
        
        char error_code_str[32];
        sprintf_simple(error_code_str, "%x", frame->error_code);
        display_write(error_code_str);
        display_write("\n");
        
        // Halt on exceptions for debugging
        for(;;);
    } else {
        // Handle IRQs
        uint8_t irq = frame->interrupt_number - IRQ_BASE;
        
        // Process IRQ
        switch(irq) {
            case 0:  // Timer
                timer_handler(frame);
                break;
            case 1:  // Keyboard
                keyboard_handler(inb(0x60));
                break;
            default:
                break;
        }
        
        // Send EOI
        pic_send_eoi(irq);
    }
}