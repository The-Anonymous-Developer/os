#ifndef CPU_H
#define CPU_H

#include <stdint.h>

// CPU control functions - implemented in cpu_asm.asm
void write_cr3(uint64_t value);
uint64_t read_cr3(void);
uint64_t read_cr2(void);  // Added for page fault address
void CLI(void);
void STI(void);
void halt(void);

#endif // CPU_H