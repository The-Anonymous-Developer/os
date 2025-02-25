#include "heap.h"
#include "../hardware/display.h"
#include "../lib/string.h"
#include <stddef.h>

static uint8_t heap_space[1024 * 1024];  // 1MB heap
static heap_block_t* free_list;
static size_t total_allocated = 0;
static size_t peak_usage = 0;

void heap_init(void) {
    // Initialize first block
    heap_block_t* initial = (heap_block_t*)heap_space;
    initial->magic = HEAP_MAGIC;
    initial->is_free = true;
    initial->size = sizeof(heap_space) - sizeof(heap_block_t);
    initial->next = NULL;
    initial->prev = NULL;
    
    free_list = initial;
}

static heap_block_t* find_free_block(size_t size) {
    heap_block_t* current = free_list;
    
    while (current) {
        if (current->is_free && current->size >= size) {
            return current;
        }
        current = current->next;
    }
    
    return NULL;
}

static void split_block(heap_block_t* block, size_t size) {
    if (block->size >= size + sizeof(heap_block_t) + MIN_BLOCK_SIZE) {
        heap_block_t* new_block = (heap_block_t*)((uint8_t*)block + sizeof(heap_block_t) + size);
        new_block->magic = HEAP_MAGIC;
        new_block->is_free = true;
        new_block->size = block->size - size - sizeof(heap_block_t);
        new_block->next = block->next;
        new_block->prev = block;
        
        if (block->next) {
            block->next->prev = new_block;
        }
        
        block->next = new_block;
        block->size = size;
    }
}

void* heap_alloc(size_t size) {
    if (size == 0) return NULL;
    
    // Align size to 8 bytes
    size = (size + 7) & ~7;
    
    heap_block_t* block = find_free_block(size);
    if (!block) return NULL;
    
    split_block(block, size);
    block->is_free = false;
    
    total_allocated += size;
    if (total_allocated > peak_usage) {
        peak_usage = total_allocated;
    }
    
    return (void*)((uint8_t*)block + sizeof(heap_block_t));
}

void heap_free(void* ptr) {
    if (!ptr) return;
    
    heap_block_t* block = (heap_block_t*)((uint8_t*)ptr - sizeof(heap_block_t));
    if (block->magic != HEAP_MAGIC) {
        display_write("HEAP ERROR: Invalid magic number in block header!\n");
        return;
    }
    
    block->is_free = true;
    total_allocated -= block->size;
    
    // Coalesce with next block if free
    if (block->next && block->next->is_free) {
        block->size += sizeof(heap_block_t) + block->next->size;
        block->next = block->next->next;
        if (block->next) {
            block->next->prev = block;
        }
    }
    
    // Coalesce with previous block if free
    if (block->prev && block->prev->is_free) {
        block->prev->size += sizeof(heap_block_t) + block->size;
        block->prev->next = block->next;
        if (block->next) {
            block->next->prev = block->prev;
        }
    }
}

void heap_dump_stats(void) {
    char buf[128];
    string_format(buf, "Heap Statistics:\n"
                      "Total Allocated: %d bytes\n"
                      "Peak Usage: %d bytes\n"
                      "Free Memory: %d bytes\n",
                      total_allocated,
                      peak_usage,
                      sizeof(heap_space) - total_allocated);
    display_write(buf);

    // Print block map
    heap_block_t* current = free_list;
    uint32_t block_count = 0;
    
    while (current) {
        string_format(buf, "Block %d: Size=%d, %s\n",
                     block_count++,
                     current->size,
                     current->is_free ? "FREE" : "USED");
        display_write(buf);
        current = current->next;
    }
}