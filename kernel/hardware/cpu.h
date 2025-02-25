#ifndef CPU_H
#define CPU_H

#include <stdint.h>

// CPU control functions
static inline void write_cr3(uint64_t value) {
    #ifdef _MSC_VER
        __asm {
            mov cr3, value
        }
    // #else
    //     asm("mov %0, %%cr3" : : "r"(value) : "memory");
    #endif
}

static inline uint64_t read_cr3(void) {
    uint64_t value;
    #ifdef _MSC_VER
        __asm {
            mov value, cr3
        }
    // #else
    //     asm("mov %%cr3, %0" : "=r"(value) : : "memory");
    #endif
    return value;
}

// CPU control functions implemented in assembly
extern void CLI(void);
extern void STI(void);
extern void halt(void);

#endif // CPU_H