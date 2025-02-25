#ifndef PIT_H
#define PIT_H

#include <stdint.h>

// PIT channel ports
#define PIT_CHANNEL0 0x40
#define PIT_CHANNEL1 0x41
#define PIT_CHANNEL2 0x42
#define PIT_COMMAND  0x43

// PIT Modes
#define PIT_MODE0      0x00    // Interrupt on Terminal Count
#define PIT_MODE1      0x02    // Hardware Retriggerable One-Shot
#define PIT_MODE2      0x04    // Rate Generator
#define PIT_MODE3      0x06    // Square Wave Generator
#define PIT_MODE4      0x08    // Software Triggered Strobe
#define PIT_MODE5      0x0A    // Hardware Triggered Strobe

void init_pit(void);
void pit_set_frequency(uint32_t frequency);

#endif // PIT_H