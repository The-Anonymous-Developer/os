#ifndef TASK_H
#define TASK_H

#include <stdint.h>
#include <stdbool.h>
#include "message_types.h"

// Task configuration
#define MAX_TASKS 64
#define TASK_NAME_LENGTH 32

// Add stack size constant
#define TASK_STACK_SIZE 4096

// Task priorities
#define PRIORITY_HIGH    0
#define PRIORITY_NORMAL  1
#define PRIORITY_LOW     2
#define PRIORITY_IDLE    3

// Time slice durations (in timer ticks)
#define TIME_SLICE_HIGH    20
#define TIME_SLICE_NORMAL  10
#define TIME_SLICE_LOW     5
#define TIME_SLICE_IDLE    1

// Task states
typedef enum {
    TASK_RUNNING = 0,
    TASK_READY = 1,
    TASK_BLOCKED = 2,
    TASK_TERMINATED = 3
} task_state_t;

// Task structure
typedef struct task {
    uint64_t rsp;                    // Stack pointer
    uint32_t id;                     // Task ID
    task_state_t state;              // Current state
    uint32_t base_priority;          // Base priority level
    uint32_t current_priority;       // Current priority (for inheritance)
    uint32_t priority;               // Current scheduling priority
    uint32_t ticks_remaining;        // Time slice remaining
    uint32_t total_ticks;           // Total execution time
    char name[TASK_NAME_LENGTH];     // Task name
    void* stack;                     // Stack pointer
    uint64_t cr3;                    // Page directory base
    struct task* next;               // Next task in list
    struct task* blocked_by;         // Task holding needed resource
    struct task* waiting_tasks[MAX_TASKS]; // Tasks waiting on this
    uint32_t waiting_count;          // Number of waiting tasks
    message_queue_t msg_queue;       // Message queue
} task_t;

// Function declarations
void task_init(void);
task_t* task_create(void (*entry)(void), uint32_t priority, const char* name);
void task_schedule(void);
task_t* task_current(void);
void task_yield(void);

#endif // TASK_H