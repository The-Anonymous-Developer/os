#include "timer.h"
#include "ports.h"
#include "pic.h"
#include "../process/task.h"

static volatile uint64_t timer_ticks = 0;

// PIT (Programmable Interval Timer) ports
#define PIT_CHANNEL0 0x40
#define PIT_CHANNEL1 0x41
#define PIT_CHANNEL2 0x42
#define PIT_COMMAND  0x43

// PIT command byte bits
#define PIT_MODE2   0x34    // Rate Generator
#define PIT_MODE3   0x36    // Square Wave Generator
#define PIT_LATCH   0x00    // Latch command

void timer_handler(interrupt_frame_t* frame) {
    (void)frame;  // Unused parameter
    timer_ticks++;
    
    // Schedule tasks every 10ms
    if (timer_ticks % 10 == 0) {
        task_schedule();
    }
    
    pic_send_eoi(0);  // Timer is IRQ0
}

void timer_init(void) {
    uint16_t divisor = TIMER_DIVIDER;
    
    // Set up PIT channel 0 in rate generator mode
    outb(PIT_COMMAND, PIT_MODE2);              // Channel 0, Rate Generator mode
    outb(PIT_CHANNEL0, divisor & 0xFF);        // Low byte
    outb(PIT_CHANNEL0, (divisor >> 8) & 0xFF); // High byte
    
    // Register timer interrupt handler at IRQ0 (INT 32)
    register_interrupt_handler(IRQ_TIMER, timer_handler);
    
    // Enable timer interrupt in PIC
    pic_unmask_irq(0);
}

uint64_t get_timer_ticks(void) {
    return timer_ticks;
}

void timer_wait(uint32_t ticks) {
    uint32_t target = timer_ticks + ticks;
    while (timer_ticks < target) {
        halt();
    }
}