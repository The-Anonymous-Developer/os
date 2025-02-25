#ifndef MEMORY_H
#define MEMORY_H

#include <stddef.h>
#include "../include/kernel.h"

void* kmalloc(size_t size);
void kfree(void* ptr);
void memory_init(void);

#endif // MEMORY_H