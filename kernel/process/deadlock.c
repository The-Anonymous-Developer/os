#include "deadlock.h"
#include "../hardware/display.h"
#include "resource_stats.h"
#include <stdbool.h>

static deadlock_state_t state;
static resource_graph_t graph;

void deadlock_init(void) {
    // Initialize detection state
    state.visited = kmalloc(MAX_TASKS * sizeof(bool));
    state.recursion_stack = kmalloc(MAX_TASKS * sizeof(bool));
    state.wait_for_graph = kmalloc(MAX_TASKS * sizeof(task_t*));
    
    // Initialize resource allocation graph
    for (uint32_t i = 0; i < MAX_TASKS; i++) {
        state.visited[i] = false;
        state.recursion_stack[i] = false;
        state.wait_for_graph[i] = NULL;
        
        for (uint32_t j = 0; j < MAX_RESOURCES; j++) {
            graph.allocation[i][j] = false;
            graph.request[i][j] = false;
        }
    }
    
    graph.resource_count = 0;
    display_write("Deadlock detection initialized\n");
}

static bool has_cycle(task_t* task, uint32_t task_id) {
    if (!state.visited[task_id]) {
        state.visited[task_id] = true;
        state.recursion_stack[task_id] = true;
        
        task_t* blocking_task = task->blocked_by;
        if (blocking_task) {
            uint32_t blocking_id = blocking_task->id;
            if (!state.visited[blocking_id]) {
                if (has_cycle(blocking_task, blocking_id)) {
                    return true;
                }
            } else if (state.recursion_stack[blocking_id]) {
                return true;
            }
        }
    }
    state.recursion_stack[task_id] = false;
    return false;
}

bool deadlock_detect(task_t* task) {
    if (!task) return false;
    
    // Reset state
    for (uint32_t i = 0; i < MAX_TASKS; i++) {
        state.visited[i] = false;
        state.recursion_stack[i] = false;
    }
    
    return has_cycle(task, task->id);
}

bool deadlock_would_cause(task_t* task, task_t* resource_owner) {
    if (!task || !resource_owner) return false;
    
    // Temporarily set up the potential blocking relationship
    task_t* original_blocked_by = task->blocked_by;
    task->blocked_by = resource_owner;
    
    // Check for deadlock
    bool would_deadlock = deadlock_detect(task);
    
    // Restore original state
    task->blocked_by = original_blocked_by;
    
    return would_deadlock;
}

void deadlock_print_state(void) {
    display_write("Current Resource Wait Graph:\n");
    for (uint32_t i = 0; i < MAX_TASKS; i++) {
        if (state.wait_for_graph[i]) {
            char buf[64];
            sprintf_simple(buf, "Task %d waiting for Task %d\n", 
                         i, state.wait_for_graph[i]->id);
            display_write(buf);
        }
    }
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