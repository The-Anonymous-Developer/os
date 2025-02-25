#ifndef DEADLOCK_H
#define DEADLOCK_H

#include "task.h"
#include <stdbool.h>

#define MAX_RESOURCES 64

// Resource allocation graph
typedef struct {
    bool allocation[MAX_TASKS][MAX_RESOURCES];  // Who holds what
    bool request[MAX_TASKS][MAX_RESOURCES];     // Who wants what
    uint32_t resource_count;                    // Number of resources
    uint32_t available[MAX_RESOURCES];          // Available resources
} allocation_graph_t;

void deadlock_init(void);
bool deadlock_detect(void);
bool deadlock_would_occur(task_t* task, uint32_t resource_id);
void deadlock_resource_acquired(task_t* task, uint32_t resource_id);
void deadlock_resource_released(task_t* task, uint32_t resource_id);

#endif // DEADLOCK_H