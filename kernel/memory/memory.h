#ifndef MEMORY_H
#define MEMORY_H

#include <stddef.h>
#include "../include/kernel.h"

struct task;
typedef struct task task_t;

void* kmalloc(size_t size);
void kfree(void* ptr);
void memory_init(void);
void* alloc_memory_for_task(task_t* task);


#endif // MEMORY_H