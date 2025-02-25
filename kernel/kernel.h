#ifndef KERNEL_H
#define KERNEL_H

#include <stdint.h>
#define KERNEL_VERSION "0.0.1"
#define KERNEL_NAME "SYSTEM"

// Other kernel functions
void kernel_main(void);
void init_memory_manager(void);
void init_process_manager(void);

#endif // KERNEL_H