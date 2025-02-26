#include "exec.h"
#include "../memory/memory.h"
#include "task.h"
#include "proc_mgmt.h"
#include "../hardware/display.h"
#include "../hardware/cpu.h"
#include "scheduler.h"
#include "../filesystem/ifs.h" // Filesystem for file loading
#include "../lib/string.h"

int exec(const char *path) {
    if (!path || !*path) {
        display_write("[EXEC] Invalid path\n");
        return -1;
    }

    display_write("[EXEC] Executing program: ");
    display_write(path);
    display_write("\n");

    // Ensure load_file function is declared and used correctly
    void *binary_data = load_file(path); 
    if (!binary_data) {
        display_write("[EXEC] Failed to load file\n");
        return -1;
    }

    // Create a new process
    task_t *new_task = task_create(NULL, PRIORITY_NORMAL, "NewProcess");
    if (!new_task) {
        display_write("[EXEC] Task creation failed\n");
        return -1;
    }

    // Allocate memory for the process
    new_task->code_segment = alloc_memory_for_task(new_task);
    if (!new_task->code_segment) {
        display_write("[EXEC] Memory allocation failed\n");
        task_exit(-1);
        return -1;
    }

    // Ensure get_file_size() is declared and implemented
    size_t file_size = get_file_size(path);
    if (file_size == 0) {
        display_write("[EXEC] File size is zero, aborting execution\n");
        task_exit(-1);
        return -1;
    }

    // Load executable into allocated memory
    memcpy(new_task->code_segment, binary_data, file_size);

    // Set up execution context
    new_task->registers.rip = (uint64_t)new_task->code_segment;
    new_task->registers.rsp = (uint64_t)new_task->stack_top;

    // Add to scheduler
    schedule_task(new_task);

    return 0; // Success
}
