#ifndef PROC_MGMT_H
#define PROC_MGMT_H

#include <stdint.h>

#define MAX_PROCESS_NAME 256

typedef enum {
    PROCESS_READY,
    PROCESS_RUNNING,
    PROCESS_BLOCKED,
    PROCESS_TERMINATED
} process_state_t;

typedef struct {
    uint32_t pid;
    char name[MAX_PROCESS_NAME];
    process_state_t state;
} Process;

void init_process_manager(void);
int create_process(const char* name);

#endif // PROC_MGMT_H
