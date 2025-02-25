#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>
#include "interrupt.h"

// Timer configuration
#define TIMER_FREQUENCY 1000    // 1000Hz = 1ms resolution
#define TIMER_DIVIDER  (1193180 / TIMER_FREQUENCY)  // PIT input clock is 1.193180 MHz

// PIT (Programmable Interval Timer) ports
#define PIT_CHANNEL0   0x40    // Channel 0 data port
#define PIT_CHANNEL1   0x41    // Channel 1 data port
#define PIT_CHANNEL2   0x42    // Channel 2 data port
#define PIT_COMMAND    0x43    // Mode/Command register

// Function declarations
void timer_init(void);
void timer_handler(interrupt_frame_t* frame);
uint32_t get_timer_ticks(void);
uint64_t get_system_ticks(void);
void timer_wait(uint32_t ticks);

#endif // TIMER_H