#include "../hardware/cpu.h"
#include "../hardware/timer.h"
#include "../hardware/display.h"
#include "deadlock.h"
#include "resource_stats.h"
#include "resource_defs.h"
#include "resource.h"

static resource_t resources[MAX_RESOURCES];

void resource_init(void) {
    for (uint32_t i = 0; i < MAX_RESOURCES; i++) {
        resources[i].id = i;
        resources[i].owner = NULL;
        resources[i].lock_count = 0;
        resources[i].wait_count = 0;
    }
}

bool resource_try_acquire(uint32_t id) {
    if (id >= MAX_RESOURCES) return false;
    
    CLI();  // Disable interrupts

    resource_t* res = &resources[id];
    task_t* current = task_current();
    bool success = false;
    uint32_t wait_time = get_system_ticks() - res->acquire_time;

    if (!res->owner || res->owner == current) {
        res->owner = current;
        res->lock_count++;
        res->acquire_time = get_system_ticks();
        success = true;
        
        // Update statistics with wait time
        resource_stats_acquire(id, wait_time);
    }

    STI();  // Enable interrupts
    return success;
}

void resource_acquire(uint32_t id) {
    while (!resource_try_acquire(id)) {
        resource_t* res = &resources[id];
        task_t* current = task_current();
        
        if (res->wait_count < MAX_TASKS) {
            res->waiting_tasks[res->wait_count++] = current;
            current->blocked_by = res->owner;
            task_set_state(current, TASK_BLOCKED);
            
            // Check for deadlock with current task
            if (deadlock_detect(current)) {
                display_write("Deadlock detected!\n");
                // TODO: Implement deadlock resolution
            }
        }
        
        task_yield();
    }
}

void resource_release(uint32_t id) {
    if (id >= MAX_RESOURCES) return;

    CLI();  // Disable interrupts

    resource_t* res = &resources[id];
    task_t* current = task_current();

    if (res->owner == current) {
        if (--res->lock_count == 0) {
            // Calculate hold time
            uint32_t hold_time = get_system_ticks() - res->acquire_time;
            
            // Update statistics
            resource_stats_release(id, hold_time);
            
            res->owner = NULL;

            // Wake up waiting task if any
            if (res->wait_count > 0) {
                task_t* waiting = res->waiting_tasks[--res->wait_count];
                waiting->blocked_by = NULL;
                task_set_state(waiting, TASK_READY);
            }
        }
    }

    STI();  // Enable interrupts
}

void resource_release_all(task_t* task) {
    for (uint32_t i = 0; i < MAX_RESOURCES; i++) {
        if (resources[i].owner == task) {
            resource_release(i);
        }
    }
}