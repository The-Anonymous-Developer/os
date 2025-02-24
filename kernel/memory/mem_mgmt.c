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

void* allocate_memory(size_t size){
    // basic memory allocate
    return (void*)0; // dummy return tbc
}

void free_memory(void* ptr){
    // mem deallocation
}