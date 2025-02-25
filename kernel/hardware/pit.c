#include "hardware.h"
#include "pit.h"
#include "ports.h"

// Base frequency of PIT is 1.193182 MHz
#define PIT_FREQUENCY   1193182
#define TIMER_FREQ      100    // 100 Hz = 10ms intervals

void init_pit(void) {
    // Calculate divisor for desired frequency
    uint32_t divisor = PIT_FREQUENCY / TIMER_FREQ;

    // Configure PIT channel 0 in mode 3 (square wave)
    outb(PIT_COMMAND, PIT_MODE3);

    // Set frequency divisor
    outb(PIT_CHANNEL0, (uint8_t)(divisor & 0xFF));         // Low byte
    outb(PIT_CHANNEL0, (uint8_t)((divisor >> 8) & 0xFF));  // High byte

    // Initialize tick counter (if needed)
    // volatile uint64_t tick_count = 0;
}