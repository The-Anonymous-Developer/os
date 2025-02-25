#include <stdint.h>
#include <stddef.h>

typedef struct {
    uint64_t total_memory;
    uint64_t used_memory;
    uint64_t free_memory;
} MemoryInfo;

void init_memory_manager(void){
    // mem mgmt init
}

void* allocate_memory(size_t size) {
    if (size == 0) return NULL;
    // TODO: Implement memory allocation
    return NULL;
}

void free_memory(void* ptr) {
    if (!ptr) return;
    // TODO: Implement memory deallocation
}