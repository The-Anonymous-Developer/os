#ifndef RESOURCE_H
#define RESOURCE_H

#include <stdint.h>
#include "task.h"
#include "resource_defs.h"

#define MAX_RESOURCE_NAME 32

typedef struct {
    uint32_t id;
    char name[MAX_RESOURCE_NAME];
    task_t* owner;
    uint32_t lock_count;
    uint64_t acquire_time;
    uint32_t wait_count;
    task_t* waiting_tasks[MAX_TASKS];
} resource_t;

void resource_init(void);
void resource_acquire(uint32_t id);
void resource_release(uint32_t id);
void resource_release_all(task_t* task);
bool resource_try_acquire(uint32_t id);
void resource_release_all(task_t* task);


#endif // RESOURCE_H