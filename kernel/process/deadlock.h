#ifndef DEADLOCK_H
#define DEADLOCK_H

#include <stdbool.h>
#include "task.h"
#include "resource_defs.h"

// Resource allocation graph structure
typedef struct {
    bool allocation[MAX_TASKS][MAX_RESOURCES];  // Current resource allocations
    bool request[MAX_TASKS][MAX_RESOURCES];     // Resource requests
    uint32_t resource_count;                    // Number of active resources
} resource_graph_t;

// Deadlock detection state
typedef struct {
    bool* visited;
    bool* recursion_stack;
    task_t** wait_for_graph;
} deadlock_state_t;

// Function declarations
void deadlock_init(void);
bool deadlock_detect(task_t* task);
bool deadlock_would_cause(task_t* task, task_t* resource_owner);
void deadlock_print_state(void);
void deadlock_resource_acquired(task_t* task, uint32_t resource_id);
void deadlock_resource_released(task_t* task, uint32_t resource_id);

#endif // DEADLOCK_H