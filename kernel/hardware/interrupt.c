#include "hardware.h"
#include "interrupt.h"
#include "ports.h"
#include "pic.h"
#include "display.h"
#include "../lib/string.h"
#include "idt.h"
#include "keyboard.h"
#include "cpu.h"
#include "timer.h"

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
    "SIMD Exception",
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

// Array of interrupt handlers
static interrupt_handler_t interrupt_handlers[256] = { 0 };

void register_interrupt_handler(uint8_t vector, interrupt_handler_t handler) {
    if (vector < MAX_INTERRUPTS-1) {
        interrupt_handlers[vector] = handler;
    }
}

void interrupt_handler(interrupt_frame_t* frame) {
    if (frame->int_no < 256 && interrupt_handlers[frame->int_no]) {
        interrupt_handlers[frame->int_no](frame);
    } else if (frame->int_no < 32) {
        // Handle CPU exceptions
        char buf[256];
        string_format(buf, "Exception: %s\nError Code: 0x%x\n",
                     exception_messages[frame->int_no],
                     frame->error_code);
        display_write(buf);
        
        halt();
    } else {
        // Handle IRQs
        uint8_t irq = frame->int_no - IRQ_BASE;
        pic_send_eoi(irq);
    }
}

static const char* exception_names[32] = {
    "Division Error", "Debug", "NMI", "Breakpoint",
    "Overflow", "Bound Range", "Invalid Opcode", "Device Not Available",
    "Double Fault", "Coprocessor Segment", "Invalid TSS", "Segment Not Present",
    "Stack Fault", "Protection Fault", "Page Fault", "Reserved",
    "FPU Error", "Alignment Check", "Machine Check", "SIMD Exception",
    "Reserved", "Reserved", "Reserved", "Reserved",
    "Reserved", "Reserved", "Reserved", "Reserved",
    "Reserved", "Reserved", "Reserved", "Reserved"
};

void default_exception_handler(interrupt_frame_t* frame) {
    char buf[256];
    string_format(buf, "Exception %d (%s)\n"
                      "Error Code: 0x%x\n"
                      "RIP: 0x%x\n"
                      "CS: 0x%x\n"
                      "RFLAGS: 0x%x\n",
                      frame->int_no,
                      get_exception_name(frame->int_no),
                      frame->error_code,
                      frame->rip,
                      frame->cs,
                      frame->rflags);
    display_write(buf);
    halt();
}

void keyboard_handler_wrapper(struct interrupt_frame* frame) {
    (void)frame;
    uint8_t scancode = inb(0x60);  // Read from keyboard port
    keyboard_handler(scancode);
}

void init_interrupt_handlers(void) {
    // Register default handlers for CPU exceptions
    for (int i = 0; i < 32; i++) {
        register_interrupt_handler(i, default_exception_handler);
    }

    // Register specific exception handlers
    register_interrupt_handler(INT_PAGE_FAULT, page_fault_handler);
    register_interrupt_handler(INT_DOUBLE_FAULT, double_fault_handler);
    register_interrupt_handler(INT_PROTECTION_FAULT, protection_fault_handler);

    // Note: Timer handler is now registered in timer_init()
    register_interrupt_handler(IRQ_KEYBOARD, keyboard_handler_wrapper);
    register_interrupt_handler(IRQ_MOUSE, mouse_handler);
    register_interrupt_handler(IRQ_HARDDISK, harddisk_handler);
}

void unregister_interrupt_handler(uint8_t int_no) {
    interrupt_handlers[int_no] = default_exception_handler;
}

const char* get_exception_name(uint8_t int_no) {
    return (int_no < 32) ? exception_messages[int_no] : "Unknown Exception";
}

// Specific handlers
void page_fault_handler(interrupt_frame_t* frame) {
    uint64_t fault_addr = read_cr2();  // Get fault address using our CPU function

    char buf[256];
    string_format(buf, "Page Fault!\n"
                      "Address: 0x%x\n"
                      "Error Code: 0x%x\n"
                      "RIP: 0x%x\n",
                      fault_addr,
                      frame->error_code,
                      frame->rip);
    display_write(buf);
    halt();
}

void double_fault_handler(interrupt_frame_t* frame) {
    char buf[256];
    string_format(buf, "Double Fault!\n"
                      "Error Code: 0x%x\n"
                      "RIP: 0x%x\n"
                      "CS: 0x%x\n"
                      "RFLAGS: 0x%x\n",
                      frame->error_code,
                      frame->rip,
                      frame->cs,
                      frame->rflags);
    display_write(buf);
    halt();
}

void protection_fault_handler(interrupt_frame_t* frame) {
    char buf[256];
    string_format(buf, "General Protection Fault!\n"
                      "Error Code: 0x%x\n"
                      "RIP: 0x%x\n"
                      "CS: 0x%x\n"
                      "RFLAGS: 0x%x\n",
                      frame->error_code,
                      frame->rip,
                      frame->cs,
                      frame->rflags);
    display_write(buf);
    halt();
}

void mouse_handler(interrupt_frame_t* frame) {
    (void)frame; // Unused parameter
    uint8_t mouse_data = inb(0x60);
    // TODO: Implement mouse data processing
    pic_send_eoi(12); // Mouse is on IRQ12
}

void harddisk_handler(interrupt_frame_t* frame) {
    (void)frame; // Unused parameter
    uint8_t status = inb(0x1F7); // Primary HDD status port
    
    if (status & 0x8) { // Check if data is ready
        // TODO: Implement disk data processing
    }
    
    pic_send_eoi(14); // HDD is on IRQ14
}