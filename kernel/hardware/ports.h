#ifndef PORTS_H
#define PORTS_H

#include <stdint.h>

// Port I/O functions implemented in assembly
extern void outb(uint16_t port, uint8_t value);
extern uint8_t inb(uint16_t port);
extern void io_wait(void);

#endif // PORTS_H