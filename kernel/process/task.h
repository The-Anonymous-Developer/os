#ifndef TASK_H
#define TASK_H

#include "../include/kernel.h"
#include "../hardware/timer.h"
#include "../memory/memory.h"
#include "../lib/string.h"
#include <stdint.h>
#include <stdbool.h>
#include "message_types.h"

// Task configuration
#define MAX_TASKS 64
#define TASK_NAME_LENGTH 32
#define TASK_STACK_SIZE 4096 // Stack size per task

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
typedef enum task_state {
    TASK_CREATED = 0,    // Task created but not yet ready
    TASK_READY = 1,      // Task ready to run
    TASK_RUNNING = 2,    // Task currently running
    TASK_BLOCKED = 3,    // Task blocked (waiting for resource/event)
    TASK_SLEEPING = 4,   // Task sleeping (waiting for time)
    TASK_ZOMBIE = 5,     // Task terminated but not yet cleaned up
    TASK_DEAD = 6        // Task fully terminated
} task_state_t;

// CPU registers structure (used for saving/restoring context)
typedef struct cpu_registers {
    uint64_t rax, rbx, rcx, rdx;
    uint64_t rsi, rdi, rbp, rsp;
    uint64_t r8, r9, r10, r11, r12, r13, r14, r15;
    uint64_t rip, rflags;
    uint64_t cr3;
    uint64_t cs, ss;
} cpu_registers_t;

// Task structure (Process Control Block)
typedef struct task {
    uint64_t pid;                // Process ID
    uint64_t rsp;
    uint64_t cr3;
    char name[TASK_NAME_LENGTH];  // Task name

    cpu_registers_t cpu_state;    // CPU context for this task
    uint64_t stack_top;           // Stack top pointer for the task
    uint64_t code_segment;        // Code segment selector (if needed for segmentation)
    
    task_state_t state;           // Current task state
    uint64_t page_table;          // CR3 (Page Table Address) for virtual memory

    struct task* next;            // Next task in linked list (for scheduling)
    struct task* prev;            // Previous task in list (if doubly linked)

    int32_t exit_code;            // Exit code for the task
    uint64_t run_start_time;      // Last time task started running
    uint64_t total_runtime;       // Total running time
    
    uint32_t base_priority;       // Base priority level
    uint32_t current_priority;    // Dynamic priority (can change)
    uint32_t priority;            // Effective priority (used for scheduling)
    uint32_t ticks_remaining;     // Time slice remaining
    uint32_t total_ticks;         // Total execution ticks

    void* stack;                  // Pointer to allocated stack
    struct task* parent;           // Parent task (if applicable)
    struct task* blocked_by;       // Task currently blocking this task
    struct task* waiting_tasks[MAX_TASKS]; // List of tasks waiting on this one
    uint32_t waiting_count;        // Number of tasks waiting

    message_queue_t msg_queue;     // Task-specific message queue (IPC)
    cpu_registers_t registers;

} task_t;

// Function declarations
void task_init(void);
task_t* task_create(void (*entry)(void), uint32_t priority, const char* name);
void task_schedule(void);
task_t* task_current(void);
void task_yield(void);
void task_exit(int exit_code);
void task_set_state(task_t* task, task_state_t new_state);
bool task_can_transition(task_state_t from, task_state_t to);
const char* task_state_name(task_state_t state);
void task_switch(task_t* next);  // Declare task switching function

#endif // TASK_H
