#include "sync.h"
#include "../hardware/interrupt.h"
#include "../hardware/cpu.h"
#include "task.h"
#include "../kernel.h"
#include "message.h"
#include "deadlock.h"
#include "resource_stats.h"
#include "benchmark.h"
#include "../hardware/timer.h"
#include "message_types.h"

void mutex_init(mutex_t* mutex, uint32_t id) {
    mutex->locked = false;
    mutex->id = id;
    mutex->acquire_time = 0;
    mutex->owner = NULL;
    mutex->waiting_count = 0;
    for (uint32_t i = 0; i < MAX_TASKS; i++) {
        mutex->waiting_tasks[i] = NULL;
    }
}

bool mutex_try_lock(mutex_t* mutex) {
    CLI();
    if (mutex->locked) {
        STI();
        return false;
    }
    mutex->locked = true;
    mutex->owner = task_current();
    mutex->acquire_time = get_system_ticks();
    deadlock_resource_acquired(mutex->owner, mutex->id);
    STI();
    return true;
}

static void inherit_priority(task_t* from, task_t* to) {
    if (!from || !to) return;
    
    if (from->current_priority < to->current_priority) {
        to->current_priority = from->current_priority;
        
        // If the mutex owner is blocked, propagate priority
        if (to->blocked_by) {
            inherit_priority(to, to->blocked_by);
        }
    }
}

static void restore_priority(task_t* task) {
    if (!task) return;
    
    // Restore to base priority
    task->current_priority = task->base_priority;
    
    // Find highest priority among waiting tasks
    for (uint32_t i = 0; i < task->waiting_count; i++) {
        if (task->waiting_tasks[i] && 
            task->waiting_tasks[i]->current_priority < task->current_priority) {
            task->current_priority = task->waiting_tasks[i]->current_priority;
        }
    }
}

void mutex_lock(mutex_t* mutex) {
    while (!mutex_try_lock(mutex)) {
        CLI();
        
        // Add current task to waiting list
        if (mutex->waiting_count < MAX_TASKS) {
            task_t* current = task_current();
            mutex->waiting_tasks[mutex->waiting_count++] = current;
            current->blocked_by = mutex->owner;
            
            // Inherit priority
            inherit_priority(current, mutex->owner);
        }
        
        task_current()->state = TASK_BLOCKED;
        STI();
        
        task_yield();
    }
}

// Update mutex unlock to use restore_priority
void mutex_unlock(mutex_t* mutex) {
    CLI();
    
    task_t* current = task_current();
    if (mutex->owner == current) {
        resource_stats_release(mutex->id, get_system_ticks() - mutex->acquire_time);
        mutex->locked = false;
        mutex->owner = NULL;
        deadlock_resource_released(current, mutex->id);
        
        // Restore priority before waking waiting tasks
        restore_priority(current);
        
        // Wake up one waiting task
        if (mutex->waiting_count > 0) {
            task_t* task = mutex->waiting_tasks[--mutex->waiting_count];
            task->blocked_by = NULL;
            task->state = TASK_READY;
        }
    }
    
    STI();
}

void semaphore_init(semaphore_t* sem, int32_t initial_count) {
    sem->count = initial_count;
    sem->waiting_count = 0;
}

bool semaphore_try_wait(semaphore_t* sem) {
    CLI();
    
    if (sem->count > 0) {
        sem->count--;
        STI();
        return true;
    }
    
    STI();
    return false;
}

void semaphore_wait(semaphore_t* sem) {
    while (!semaphore_try_wait(sem)) {
        // Add current task to waiting list
        if (sem->waiting_count < MAX_TASKS) {
            sem->waiting_tasks[sem->waiting_count++] = task_current();
        }
        
        // Set task state to blocked
        task_current()->state = TASK_BLOCKED;
        
        // Yield to other tasks
        task_yield();
    }
}

void semaphore_signal(semaphore_t* sem) {
    CLI();
    
    sem->count++;
    
    // Wake up one waiting task
    if (sem->waiting_count > 0) {
        task_t* task = sem->waiting_tasks[--sem->waiting_count];
        task->state = TASK_READY;
    }
    
    STI();
}

void rmutex_init(rmutex_t* mutex) {
    mutex->locked = false;
    mutex->owner = NULL;
    mutex->lock_count = 0;
    mutex->waiting_count = 0;
}

bool rmutex_try_lock(rmutex_t* mutex) {
    CLI();
    
    task_t* current = task_current();
    
    // If not locked or we own it
    if (!mutex->locked || mutex->owner == current) {
        mutex->locked = true;
        mutex->owner = current;
        mutex->lock_count++;
        STI();
        return true;
    }
    
    STI();
    return false;
}

void rmutex_lock(rmutex_t* mutex) {
    while (!rmutex_try_lock(mutex)) {
        // Add current task to waiting list if not already waiting
        if (mutex->waiting_count < MAX_TASKS) {
            mutex->waiting_tasks[mutex->waiting_count++] = task_current();
        }
        
        // Set task state to blocked
        task_current()->state = TASK_BLOCKED;
        
        // Yield to other tasks
        task_yield();
    }
}

void rmutex_unlock(rmutex_t* mutex) {
    CLI();
    
    if (mutex->owner == task_current()) {
        mutex->lock_count--;
        
        // Only unlock if all recursive locks are released
        if (mutex->lock_count == 0) {
            mutex->locked = false;
            mutex->owner = NULL;
            
            // Wake up one waiting task
            if (mutex->waiting_count > 0) {
                task_t* task = mutex->waiting_tasks[--mutex->waiting_count];
                task->state = TASK_READY;
            }
        }
    }
    
    STI();
}

// Example task function showing recursive mutex usage
void nested_task(rmutex_t* mutex) {
    rmutex_lock(mutex);    // First lock
    
    // Do some work...
    
    rmutex_lock(mutex);    // Second lock (same task)
    
    // Do more work...
    
    rmutex_unlock(mutex);  // Release second lock
    rmutex_unlock(mutex);  // Release first lock
}

void condition_init(condition_t* cond) {
    cond->waiting_count = 0;
}

void condition_wait(condition_t* cond, mutex_t* mutex) {
    CLI();
    
    // Add current task to condition's wait queue
    if (cond->waiting_count < MAX_TASKS) {
        cond->waiting_tasks[cond->waiting_count++] = task_current();
    }
    
    // Release mutex and block
    mutex_unlock(mutex);
    task_current()->state = TASK_BLOCKED;
    
    STI();
    
    // Yield CPU to other tasks
    task_yield();
    
    // Reacquire mutex when we wake up
    mutex_lock(mutex);
}

void condition_signal(condition_t* cond) {
    CLI();
    
    // Wake up one waiting task
    if (cond->waiting_count > 0) {
        task_t* task = cond->waiting_tasks[--cond->waiting_count];
        task->state = TASK_READY;
    }
    
    STI();
}

void condition_broadcast(condition_t* cond) {
    CLI();
    
    // Wake up all waiting tasks
    while (cond->waiting_count > 0) {
        task_t* task = cond->waiting_tasks[--cond->waiting_count];
        task->state = TASK_READY;
    }
    
    STI();
}

void rwlock_init(rwlock_t* lock) {
    mutex_init(&lock->mutex, RWLOCK_MUTEX_ID);
    condition_init(&lock->read_ok);
    condition_init(&lock->write_ok);
    lock->readers = 0;
    lock->writer = false;
    lock->waiting_writers = 0;
}

void rwlock_read_lock(rwlock_t* lock) {
    mutex_lock(&lock->mutex);
    
    // Wait while there's a writer or waiting writers
    while (lock->writer || lock->waiting_writers > 0) {
        condition_wait(&lock->read_ok, &lock->mutex);
    }
    
    lock->readers++;
    mutex_unlock(&lock->mutex);
}

void rwlock_read_unlock(rwlock_t* lock) {
    mutex_lock(&lock->mutex);
    lock->readers--;
    
    // If last reader, signal a waiting writer
    if (lock->readers == 0 && lock->waiting_writers > 0) {
        condition_signal(&lock->write_ok);
    }
    
    mutex_unlock(&lock->mutex);
}

void rwlock_write_lock(rwlock_t* lock) {
    mutex_lock(&lock->mutex);
    lock->waiting_writers++;
    
    // Wait while there are readers or another writer
    while (lock->readers > 0 || lock->writer) {
        condition_wait(&lock->write_ok, &lock->mutex);
    }
    
    lock->waiting_writers--;
    lock->writer = true;
    mutex_unlock(&lock->mutex);
}

void rwlock_write_unlock(rwlock_t* lock) {
    mutex_lock(&lock->mutex);
    lock->writer = false;
    
    // If there are waiting writers, wake one
    if (lock->waiting_writers > 0) {
        condition_signal(&lock->write_ok);
    } else {
        // Otherwise wake all readers
        condition_broadcast(&lock->read_ok);
    }
    
    mutex_unlock(&lock->mutex);
}