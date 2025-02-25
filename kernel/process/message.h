#ifndef MESSAGE_H
#define MESSAGE_H

#include <stdbool.h>
#include "message_types.h"
#include "task.h"

#define MESSAGE_MUTEX_ID 1000

// Function declarations
void message_system_init(void);
void message_queue_init(message_queue_t* queue, struct task* owner);
bool message_send(struct task* dest, message_t* msg);
bool message_receive(message_t* msg);
bool message_try_receive(message_t* msg);

#endif // MESSAGE_H