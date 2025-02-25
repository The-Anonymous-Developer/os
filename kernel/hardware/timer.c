#include "timer.h"
#include "pic.h"
#include "interrupt.h"
#include "ports.h"
#include "cpu.h"
#include "../process/task.h"

static volatile uint32_t timer_ticks = 0;

// PIT (Programmable Interval Timer) ports
#define PIT_CHANNEL0 0x40
#define PIT_CHANNEL1 0x41
#define PIT_CHANNEL2 0x42
#define PIT_COMMAND  0x43

void timer_handler(interrupt_frame_t* frame) {
    (void)frame;  // Unused parameter
    timer_ticks++;
    
    // Task scheduling
    if (timer_ticks % 10 == 0) {  // Every 10ms
        task_schedule();
    }
}

void timer_init(void) {
    uint16_t divisor = TIMER_DIVIDER;
    
    // Set up PIT channel 0 in rate generator mode
    outb(PIT_COMMAND, 0x36);   // Channel 0, Rate Generator mode
    outb(PIT_CHANNEL0, divisor & 0xFF);        // Low byte
    outb(PIT_CHANNEL0, (divisor >> 8) & 0xFF); // High byte
    
    // Register timer interrupt handler at IRQ0 (INT 32)
    register_interrupt_handler(IRQ0, timer_handler);
    
    // Enable timer interrupt in PIC
    pic_unmask_irq(0);
}

uint32_t get_timer_ticks(void) {
    return timer_ticks;
}

uint64_t get_system_ticks(void) {
    return (uint64_t)timer_ticks;
}

void timer_wait(uint32_t ticks) {
    uint32_t target = timer_ticks + ticks;
    while (timer_ticks < target) {
        halt();
    }
}