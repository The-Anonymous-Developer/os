#include <stdint.h>
#include "../kernel.h"
#include "../hardware/hardware.h"
#include "../hardware/display.h"
#include "../hardware/keyboard.h"
#include "../process/task.h"


// Function declarations
static void init_system_components(void);
static void schedule_next_task(void);
static void process_system_events(void);
static void update_ai_system(void);
static void check_power_state(void);
static void init_drivers(void);
static void init_filesystem(void);
static void init_network(void);
static void init_ai_system(void);

// External functions from other modules
extern void init_memory_manager(void);
extern void init_process_manager(void);
extern void init_hardware(void);

// Kernel entry point alignment
#ifdef _MSC_VER
    #define KERNEL_ENTRY void __declspec(align(16))
#else
    #define KERNEL_ENTRY void __attribute__((aligned(16)))
#endif

KERNEL_ENTRY _start(void) {
    // Disable interrupts during initialization
    #ifdef _MSC_VER
        __asm {
            cli
        }
    #else
        asm volatile ("cli");
    #endif
    
    // Initialize essential kernel components
    init_hardware();
    init_memory_manager();
    init_process_manager();
    
    // Jump to kernel main
    kernel_main();
}

void kernel_main(void) {
    // Initialize system components
    init_system_components();
    
    // Initialize display
    display_init();
    display_set_color(VGA_WHITE, VGA_BLUE);
    display_write("OS Kernel Started\n");
    display_write("Ready for input:\n");
    
    // Enable interrupts
    #ifdef _MSC_VER
        __asm {
            sti
        }
    // #else
    //     asm volatile ("sti");
    #endif
    
    // Main kernel loop
    while(1) {
        // Process keyboard input
        if (keyboard_available()) {
            char c = keyboard_getchar();
            display_putchar(c);
        }
        
        // Process scheduler tick
        schedule_next_task();
        
        // Handle system events
        process_system_events();
        
        // AI system monitor tick
        update_ai_system();
        
        // Power management
        check_power_state();
    }
}

static void init_system_components(void) {
    // Initialize device drivers
    init_drivers();
    
    // Initialize filesystem
    init_filesystem();
    
    // Initialize network stack
    init_network();
    
    // Initialize AI subsystem
    init_ai_system();
    
    // Initialize task system
    task_init();
}

static void schedule_next_task(void) {
    task_schedule();
    task_t* current = task_current();
    if (current) {
        // Update AI system with task metrics
        // TODO: Implement AI-driven scheduling
    }
}

static void process_system_events(void) {
    // Handle pending system events
    // Will be implemented in event manager
}

static void update_ai_system(void) {
    // Update AI system state
    // Will be implemented in AI core
}

static void check_power_state(void) {
    // Check and adjust power states
    // Will be implemented in power manager
}

static void init_drivers(void) {
    // Initialize essential drivers
    // Will be implemented in driver manager
}

static void init_filesystem(void) {
    // Initialize filesystem
    // Will be implemented in filesystem manager
}

static void init_network(void) {
    // Initialize network stack
    // Will be implemented in network manager
}

static void init_ai_system(void) {
    // Initialize AI subsystem
    // Will be implemented in AI core
}