#ifndef CPU_H
#define CPU_H

#include <stdint.h>

// CPU control functions implemented in assembly
extern void CLI(void);
extern void STI(void);
extern void halt(void);

#endif // CPU_H