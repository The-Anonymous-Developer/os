#include "deadlock.h"
#include "../hardware/display.h"

static allocation_graph_t graph;

void deadlock_init(void) {
    graph.resource_count = 0;
    for (uint32_t i = 0; i < MAX_TASKS; i++) {
        for (uint32_t j = 0; j < MAX_RESOURCES; j++) {
            graph.allocation[i][j] = false;
            graph.request[i][j] = false;
        }
    }
    display_write("Deadlock detection initialized\n");
}

static bool is_cycle_util(uint32_t task_id, bool visited[], bool *stack) {
    if (!visited[task_id]) {
        visited[task_id] = true;
        stack[task_id] = true;

        // Check all resources this task has allocated
        for (uint32_t res = 0; res < graph.resource_count; res++) {
            if (graph.allocation[task_id][res]) {
                // Find all tasks requesting this resource
                for (uint32_t t = 0; t < MAX_TASKS; t++) {
                    if (graph.request[t][res]) {
                        if (!visited[t] && is_cycle_util(t, visited, stack)) {
                            return true;
                        } else if (stack[t]) {
                            return true;
                        }
                    }
                }
            }
        }
    }
    stack[task_id] = false;
    return false;
}

bool deadlock_detect(void) {
    bool visited[MAX_TASKS] = {false};
    bool stack[MAX_TASKS] = {false};

    for (uint32_t i = 0; i < MAX_TASKS; i++) {
        if (is_cycle_util(i, visited, stack)) {
            display_write("Deadlock detected!\n");
            return true;
        }
    }
    return false;
}

bool deadlock_would_occur(task_t* task, uint32_t resource_id) {
    if (!task || resource_id >= MAX_RESOURCES) return true;

    // Temporarily mark the resource as requested
    graph.request[task->id][resource_id] = true;
    
    bool would_deadlock = deadlock_detect();
    
    // Undo temporary marking
    graph.request[task->id][resource_id] = false;
    
    return would_deadlock;
}

void deadlock_resource_acquired(task_t* task, uint32_t resource_id) {
    if (!task || resource_id >= MAX_RESOURCES) return;
    
    graph.request[task->id][resource_id] = false;
    graph.allocation[task->id][resource_id] = true;
    
    if (resource_id >= graph.resource_count) {
        graph.resource_count = resource_id + 1;
    }
}

void deadlock_resource_released(task_t* task, uint32_t resource_id) {
    if (!task || resource_id >= MAX_RESOURCES) return;
    
    graph.allocation[task->id][resource_id] = false;
}