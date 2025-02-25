#ifndef KERNEL_H
#define KERNEL_H

// Standard includes
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// System-wide constants
#define MAX_TASKS       64
#define MAX_RESOURCES   64
#define STACK_SIZE      4096
#define HEAP_SIZE       (1024 * 1024)  // 1MB heap

// Error codes
typedef enum {
    SUCCESS = 0,
    ERR_NULL_POINTER = -1,
    ERR_OUT_OF_MEMORY = -2,
    ERR_INVALID_PARAM = -3,
    ERR_NOT_FOUND = -4
} error_t;

#endif // KERNEL_H