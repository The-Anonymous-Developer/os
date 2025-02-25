#include <stdint.h>

typedef struct {
    uint32_t pid;
    char name[256];
    uint32_t state;
} Process;

void init_process_manager(void){
    // proc mgmt init
}

int create_process(const char* name) {
    (void)name;  // Mark as intentionally unused
    // TODO: Implement process creation
    return -1;
}