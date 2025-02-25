#include "message.h"
#include "task.h"
#include "sync.h"
#include "../hardware/cpu.h"
#include "../memory/memory.h"
#include "../lib/string.h"

#define MAX_MESSAGES 256
#define MAX_MESSAGE_LENGTH 128

static char message_buffer[MAX_MESSAGES][MAX_MESSAGE_LENGTH];
static size_t message_head = 0;
static size_t message_tail = 0;
static mutex_t message_mutex;
static bool message_system_initialized = false;
static condition_t message_available;

void message_system_init(void) {
    // Initialize mutex and condition variable
    mutex_init(&message_mutex, MESSAGE_MUTEX_ID);
    condition_init(&message_available);
    message_system_initialized = true;
}

void message_queue_init(message_queue_t* queue, task_t* owner) {
    queue->read_index = 0;
    queue->write_index = 0;
    queue->count = 0;
    queue->owner = owner;
}

bool message_send(struct task* dest, message_t* msg) {
    if (!dest || !msg) return false;
    
    mutex_lock(&message_mutex);
    
    message_queue_t* queue = &dest->msg_queue;
    bool success = false;
    
    if (queue->count < MAX_MESSAGES) {
        // Copy message to queue
        queue->messages[queue->write_index] = *msg;
        queue->write_index = (queue->write_index + 1) % MAX_MESSAGES;
        queue->count++;
        success = true;
        
        // Signal waiting receivers
        condition_signal(&message_available);
    }
    
    mutex_unlock(&message_mutex);
    return success;
}

void send_message(const char* msg) {
    if (!message_system_initialized) {
        return;
    }

    mutex_lock(&message_mutex);
    
    // Copy message to buffer
    strncpy(message_buffer[message_tail], msg, MAX_MESSAGE_LENGTH - 1);
    message_buffer[message_tail][MAX_MESSAGE_LENGTH - 1] = '\0';
    
    // Update tail
    message_tail = (message_tail + 1) % MAX_MESSAGES;
    
    mutex_unlock(&message_mutex);
}

bool message_receive(message_t* msg) {
    if (!msg) return false;
    
    mutex_lock(&message_mutex);
    
    message_queue_t* queue = &task_current()->msg_queue;
    
    // Wait for message if queue is empty
    while (queue->count == 0) {
        condition_wait(&message_available, &message_mutex);
    }
    
    // Copy message from queue
    *msg = queue->messages[queue->read_index];
    queue->read_index = (queue->read_index + 1) % MAX_MESSAGES;
    queue->count--;
    
    mutex_unlock(&message_mutex);
    return true;
}

const char* receive_message(void) {
    if (!message_system_initialized) {
        return NULL;
    }

    static char result[MAX_MESSAGE_LENGTH];
    
    mutex_lock(&message_mutex);
    
    if (message_head == message_tail) {
        mutex_unlock(&message_mutex);
        return NULL;
    }
    
    // Copy message from buffer
    strncpy(result, message_buffer[message_head], MAX_MESSAGE_LENGTH);
    message_head = (message_head + 1) % MAX_MESSAGES;
    
    mutex_unlock(&message_mutex);
    return result;
}

bool message_try_receive(message_t* msg) {
    if (!msg) return false;
    
    mutex_lock(&message_mutex);
    
    message_queue_t* queue = &task_current()->msg_queue;
    
    if (queue->count > 0) {
        *msg = queue->messages[queue->read_index];
        queue->read_index = (queue->read_index + 1) % MAX_MESSAGES;
        queue->count--;
        mutex_unlock(&message_mutex);
        return true;
    }
    
    mutex_unlock(&message_mutex);
    return false;
}