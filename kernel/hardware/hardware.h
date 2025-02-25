#ifndef HARDWARE_H
#define HARDWARE_H

// Include necessary headers but avoid redefining structures
#include "gdt.h"
#include "idt.h"
#include "pit.h"
#include "pic.h"
#include "ports.h"

// Hardware initialization functions
void init_hardware(void);

#endif // HARDWARE_H