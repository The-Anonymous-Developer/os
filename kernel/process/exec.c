#include "exec.h"
#include "../memory/memory.h"
#include "task.h"
#include "proc_mgmt.h"
#include "../hardware/display.h"
#include "../hardware/cpu.h"
#include "scheduler.h"
#include "../filesystem/fs.h" // Placeholder for future file loading

int exec(const char *path) {
    if (!path || !*path) {
        display_write("exec: Invalid path\n");
        return -1;
    }

    display_write("Executing program: ");
    display_write(path);
    display_write("\n");

    // Placeholder: Read binary from disk (filesystem integration required)
    void *binary_data = load_file(path);
    if (!binary_data) {
        display_write("exec: Failed to load file\n");
        return -1;
    }

    // Create a new process
    task_t *new_task = create_task();
    if (!new_task) {
        display_write("exec: Task creation failed\n");
        return -1;
    }

    // Allocate memory for the process
    new_task->memory = alloc_memory_for_task(new_task);
    if (!new_task->memory) {
        display_write("exec: Memory allocation failed\n");
        destroy_task(new_task);
        return -1;
    }

    // Load executable into allocated memory
    memcpy(new_task->memory->code_segment, binary_data, get_file_size(path));

    // Set up stack pointer, instruction pointer (entry point)
    new_task->registers.rip = (uint64_t)new_task->memory->code_segment;
    new_task->registers.rsp = (uint64_t)new_task->memory->stack_top;

    // Add to scheduler
    schedule_task(new_task);

    return 0; // Success
}
