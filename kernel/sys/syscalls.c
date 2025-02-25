#include "syscalls.h"
#include "../process/exec.h"
#include "../hardware/display.h"

void syscall_handler() {
    display_write("System call invoked!\n");
    // TODO: Implement system call dispatch
}
