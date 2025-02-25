#ifndef PIT_H
#define PIT_H

#include <stdint.h>

// PIT channel ports
#define PIT_CHANNEL0 0x40
#define PIT_CHANNEL1 0x41
#define PIT_CHANNEL2 0x42
#define PIT_COMMAND  0x43

// PIT control bits
#define PIT_BINARY     0x00
#define PIT_BCD        0x01
#define PIT_MODE0      0x00
#define PIT_MODE2      0x04
#define PIT_MODE3      0x06
#define PIT_MODE4      0x08
#define PIT_MODE5      0x0A

void init_pit(void);
void pit_set_frequency(uint32_t frequency);

#endif // PIT_H