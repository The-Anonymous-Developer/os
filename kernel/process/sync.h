#ifndef SYNC_H
#define SYNC_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "task.h"

// Forward declaration
struct task;

// Constants
#define MAX_WAITING_TASKS 64
#define MAX_TASKS 64
#define RWLOCK_MUTEX_ID 2000  // Base ID for reader-writer lock mutexes

// Mutex structure
typedef struct {
    volatile bool locked;
    uint32_t id;           // Resource identifier
    uint64_t acquire_time; // Time when mutex was acquired
    struct task* owner;
    struct task* waiting_tasks[MAX_TASKS];
    uint32_t waiting_count;
} mutex_t;

// Recursive mutex structure
typedef struct {
    bool locked;
    struct task* owner;
    uint32_t lock_count;    // Number of times locked
    struct task* waiting_tasks[MAX_WAITING_TASKS];
    uint32_t waiting_count;
} rmutex_t;

// Semaphore structure
typedef struct {
    int32_t count;
    struct task* waiting_tasks[MAX_WAITING_TASKS];
    uint32_t waiting_count;
} semaphore_t;

// Condition variable structure
typedef struct {
    struct task* waiting_tasks[MAX_WAITING_TASKS];
    uint32_t waiting_count;
} condition_t;

// Reader-writer lock structure
typedef struct {
    mutex_t mutex;
    condition_t read_ok;
    condition_t write_ok;
    int32_t readers;
    bool writer;
    int32_t waiting_writers;
} rwlock_t;

// Function declarations
void mutex_init(mutex_t* mutex, uint32_t id);
bool mutex_try_lock(mutex_t* mutex);
void mutex_lock(mutex_t* mutex);
void mutex_unlock(mutex_t* mutex);

void rmutex_init(rmutex_t* mutex);
bool rmutex_try_lock(rmutex_t* mutex);
void rmutex_lock(rmutex_t* mutex);
void rmutex_unlock(rmutex_t* mutex);

void semaphore_init(semaphore_t* sem, int32_t initial_count);
void semaphore_wait(semaphore_t* sem);
bool semaphore_try_wait(semaphore_t* sem);
void semaphore_signal(semaphore_t* sem);

void condition_init(condition_t* cond);
void condition_wait(condition_t* cond, mutex_t* mutex);
void condition_signal(condition_t* cond);
void condition_broadcast(condition_t* cond);

void rwlock_init(rwlock_t* lock);
void rwlock_read_lock(rwlock_t* lock);
void rwlock_read_unlock(rwlock_t* lock);
void rwlock_write_lock(rwlock_t* lock);
void rwlock_write_unlock(rwlock_t* lock);

#endif // SYNC_H