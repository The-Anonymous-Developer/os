#ifndef TIMER_H
#define TIMER_H

#include "interrupt.h"  
#include <stdint.h>


// Timer configuration
#define TIMER_FREQUENCY 1000  // 1000 Hz = 1ms per tick
#define PIT_FREQUENCY  1193180
#define TIMER_DIVIDER (PIT_FREQUENCY / TIMER_FREQUENCY)

void timer_init(void);
uint64_t get_timer_ticks(void);  // Added declaration
void timer_wait(uint32_t ticks);
void timer_handler(interrupt_frame_t* frame);
uint64_t get_system_ticks(void);


#endif // TIMER_H