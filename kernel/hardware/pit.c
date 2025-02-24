#include "hardware.h"

#define PIT_CHANNEL0    0x40    // Channel 0 data port
#define PIT_CHANNEL1    0x41    // Channel 1 data port
#define PIT_CHANNEL2    0x42    // Channel 2 data port
#define PIT_COMMAND     0x43    // Mode/Command register

// Base frequency of PIT is 1.193182 MHz
#define PIT_FREQUENCY   1193182
#define TIMER_FREQ      100    // 100 Hz = 10ms intervals

// PIT command bits
#define PIT_MODE2      0x34    // Rate Generator
#define PIT_MODE3      0x36    // Square Wave Generator
#define PIT_LATCH      0x00    // Latch count value command

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