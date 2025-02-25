#ifndef MESSAGE_TYPES_H
#define MESSAGE_TYPES_H

#include <stdint.h>
#include <stddef.h>

// Message system constants
#define MAX_MESSAGES 256
#define MAX_MESSAGE_LENGTH 128

// Message structure
typedef struct message {
    uint32_t type;
    uint32_t sender_id;
    uint32_t priority;
    char data[MAX_MESSAGE_LENGTH];
} message_t;

// Message queue structure
typedef struct message_queue {
    message_t messages[MAX_MESSAGES];
    size_t read_index;
    size_t write_index;
    size_t count;
    struct task* owner;
} message_queue_t;

#endif // MESSAGE_TYPES_H