#include "memory.h"
#include "heap.h"
#include "../process/task.h"

#define HEAP_SIZE (1024 * 1024)  // 1MB heap

// Cross-platform alignment macro
#ifdef _MSC_VER
    #define ALIGNED(x) __declspec(align(x))
#else
    #define ALIGNED(x) __attribute__((aligned(x)))
#endif

// Aligned kernel heap
ALIGNED(8) static uint8_t kernel_heap[HEAP_SIZE];
static size_t heap_ptr = 0;

void* kmalloc(size_t size) {
    if (size == 0) return NULL;
    
    // Align size to 8 bytes
    size = (size + 7) & ~7;
    
    if (heap_ptr + size > HEAP_SIZE) {
        return NULL;  // Out of memory
    }

    void* ptr = &kernel_heap[heap_ptr];
    heap_ptr += size;
    return ptr;
}

void kfree(void* ptr) {
    (void)ptr;  // TODO: Implement proper free
}

void* alloc_memory_for_task(task_t* task) {
    if (!task) return NULL;
    
    task->code_segment = kmalloc(TASK_STACK_SIZE); // Allocate executable memory
    task->stack_top = (uint8_t*)kmalloc(TASK_STACK_SIZE) + TASK_STACK_SIZE; // Allocate stack memory

    if (!task->code_segment || !task->stack_top) {
        return NULL; // Allocation failure
    }

    return task->code_segment;
}