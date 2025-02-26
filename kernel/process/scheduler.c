#include "scheduler.h"
#include "task.h"
#include "../hardware/display.h"

void schedule() {
    display_write("Scheduling next task...\n");
    // TODO: Implement scheduling algorithm
}

void schedule_task(task_t* task) {
    if (!task) return;
    
    task->state = TASK_READY;
    add_task_to_queue(task);  // Ensure `add_task_to_queue()` exists!
}

static task_t* task_queue[MAX_TASKS];
static int queue_size = 0;

void add_task_to_queue(task_t* task){
    if (!task || queue_size >= MAX_TASKS) return;
    task_queue[queue_size++] = task;
}