#include "exec.h"
#include "../memory/memory.h"
#include "task.h"
#include "../hardware/display.h"

int exec(const char *path) {
    display_write("Executing program: ");
    display_write(path);
    display_write("\n");
    
    // TODO: Load binary into memory and switch task context
    return -1; // Return failure until implemented
}
