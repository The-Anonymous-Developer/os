#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

// Timer configuration
#define TIMER_FREQUENCY    1000    // 1000 Hz = 1ms per tick
#define PIT_FREQUENCY     1193180  // PIT base frequency (1.193180 MHz)
#define TIMER_DIVIDER     (PIT_FREQUENCY / TIMER_FREQUENCY)

// Function declarations
void timer_init(void);
uint32_t get_timer_ticks(void);
uint64_t get_system_ticks(void);
void timer_wait(uint32_t ticks);

#endif // TIMER_H