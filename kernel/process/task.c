#include "task.h"
#include "message.h"
#include <stdint.h>
#include "../hardware/display.h"
#include <stddef.h>
#include "resource_stats.h"
#include "resource_opt.h"
#include "sync.h"
#include "resource_stats.h"
#include "resource_predict.h"

// Task list and current task (make current_task visible to assembly)
static task_t tasks[MAX_TASKS];
task_t* current_task = NULL;  // Changed from static to global
static uint32_t next_task_id = 0;
static uint32_t task_count = 0;

void task_init(void) {
    task_count = 0;
    next_task_id = 0;
    current_task = NULL;
    display_write("Task system initialized\n");
}

static uint32_t get_time_slice(uint32_t priority) {
    switch (priority) {
        case PRIORITY_HIGH:   return TIME_SLICE_HIGH;
        case PRIORITY_NORMAL: return TIME_SLICE_NORMAL;
        case PRIORITY_LOW:    return TIME_SLICE_LOW;
        case PRIORITY_IDLE:   return TIME_SLICE_IDLE;
        default:             return TIME_SLICE_NORMAL;
    }
}

task_t* task_create(void (*entry)(void), uint32_t priority, const char* name) {
    if (task_count >= MAX_TASKS) {
        return NULL;
    }
    
    task_t* task = &tasks[task_count];
    
    // Allocate stack memory
    task->stack = kmalloc(TASK_STACK_SIZE);
    if (!task->stack) {
        return NULL;
    }
    
    task->id = next_task_id++;
    task->base_priority = priority;
    task->current_priority = priority;
    task->priority = priority;
    task->blocked_by = NULL;
    task->waiting_count = 0;
    task->ticks_remaining = get_time_slice(priority);
    task->total_ticks = 0;
    task->state = TASK_READY;
    
    // Copy task name
    strncpy(task->name, name, TASK_NAME_LENGTH - 1);
    task->name[TASK_NAME_LENGTH - 1] = '\0';
    
    // Setup stack (stack grows downward)
    uint64_t* stack = (uint64_t*)((uint8_t*)task->stack + TASK_STACK_SIZE);
    
    // Push initial register values onto stack
    *(--stack) = 0x200;    // RFLAGS (interrupts enabled)
    *(--stack) = 0x08;     // CS
    *(--stack) = (uint64_t)entry;  // RIP
    *(--stack) = 0;        // RAX
    *(--stack) = 0;        // RBX
    *(--stack) = 0;        // RCX
    *(--stack) = 0;        // RDX
    *(--stack) = 0;        // RSI
    *(--stack) = 0;        // RDI
    *(--stack) = 0;        // RBP
    
    task->rsp = (uint64_t)stack;
    task->cr3 = 0;  // Will be set up by memory management later
    
    // Initialize message queue
    message_queue_init(&task->msg_queue, task);
    
    task_count++;
    return task;
}

void context_switch(task_t* next) {
    if (!next || next == current_task) {
        return;
    }

    task_t* prev = current_task;
    current_task = next;
    
    // Display task switch debug info
    display_write("Switching from task ");
    display_write(prev ? prev->name : "NULL");
    display_write(" to ");
    display_write(next->name);
    display_write("\n");

    task_switch(next);
}

void task_schedule(void) {
    if (!current_task) {
        if (task_count > 0) {
            current_task = &tasks[0];
        }
        return;
    }
    
    // Decrement current task's time slice
    if (current_task->ticks_remaining > 0) {
        current_task->ticks_remaining--;
        current_task->total_ticks++;
        
        // If time slice isn't expired, continue with current task
        if (current_task->ticks_remaining > 0) {
            return;
        }
    }

    // Find highest priority ready task
    task_t* next_task = NULL;
    uint32_t highest_priority = PRIORITY_IDLE + 1;

    for (uint32_t i = 0; i < task_count; i++) {
        if (tasks[i].state == TASK_READY && tasks[i].priority < highest_priority) {
            highest_priority = tasks[i].priority;
            next_task = &tasks[i];
        }
    }

    // If no ready task found, continue with current
    if (!next_task) {
        next_task = current_task;
    }

    // Reset time slice and switch
    next_task->ticks_remaining = get_time_slice(next_task->priority);
    context_switch(next_task);
    
    // Update resource stats every scheduling cycle
    for (uint32_t i = 0; i < MAX_RESOURCES; i++) {
        resource_stats_update_history(i);
    }
    
    // Check resource alerts periodically
    static uint32_t alert_counter = 0;
    if (++alert_counter >= 100) {
        resource_stats_check_alerts();
        alert_counter = 0;
    }
    
    // Update resource stats and check alerts
    static uint32_t opt_counter = 0;
    if (++opt_counter >= 500) {  // Analyze every 500 ticks
        resource_opt_analyze();
        
        // Update and show predictions for active resources
        for (uint32_t i = 0; i < MAX_RESOURCES; i++) {
            if (stats[i].current_usage > 0) {
                resource_predict_update(i);
                resource_predict_print(i);
            }
        }
        
        uint32_t count;
        optimization_suggestion_t* suggestions = resource_opt_get_suggestions(&count);
        
        if (count > 0) {
            display_write("\nResource Optimization Suggestions:\n");
            for (uint32_t i = 0; i < count; i++) {
                display_write(suggestions[i].description);
                display_write("\n");
            }
        }
        
        opt_counter = 0;
    }
}

task_t* task_current(void) {
    return current_task;
}

void task_yield(void) {
    task_schedule();
}
