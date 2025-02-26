#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "task.h"

void schedule();
void schedule_task(task_t* task);
void add_task_to_queue(task_t* task);


#endif // SCHEDULER_H
