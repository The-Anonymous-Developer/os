#ifndef HEAP_H
#define HEAP_H

#include <stdint.h>
#include <stdbool.h>
#include "../lib/string.h"  // For size_t definition

#define HEAP_MAGIC 0x123890AB
#define MIN_BLOCK_SIZE 16
#define HEAP_BLOCKS 1024

typedef struct heap_block {
    uint32_t magic;          // For validation
    bool is_free;           // Block status
    size_t size;           // Size including header
    struct heap_block* next;
    struct heap_block* prev;
} heap_block_t;

void heap_init(void);
void* heap_alloc(size_t size);
void heap_free(void* ptr);
void heap_dump_stats(void);

#endif // HEAP_H