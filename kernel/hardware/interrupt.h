#ifndef INTERRUPT_H
#define INTERRUPT_H

#include <stdint.h>
#include <stdbool.h>

// IRQ definitions
#define IRQ0    32  // Timer
#define IRQ1    33  // Keyboard
#define IRQ2    34  // Cascade for 8259A Slave
#define IRQ3    35  // COM2
#define IRQ4    36  // COM1
#define IRQ5    37  // LPT2
#define IRQ6    38  // Floppy
#define IRQ7    39  // LPT1
#define IRQ8    40  // CMOS RTC
#define IRQ9    41  // Free
#define IRQ10   42  // Free
#define IRQ11   43  // Free
#define IRQ12   44  // PS/2 Mouse
#define IRQ13   45  // FPU
#define IRQ14   46  // Primary ATA
#define IRQ15   47  // Secondary ATA

// Cross-platform structure packing
#ifdef _MSC_VER
#define PACKED
#pragma pack(push, 1)
#else
#define PACKED __attribute__((packed))
#endif

// Interrupt state management
#define IRQ_BASE 32
#define MAX_INTERRUPTS 256

// Forward declaration of the interrupt frame structure
struct interrupt_frame;

// Function pointer type for interrupt handlers
typedef void (*interrupt_handler_t)(struct interrupt_frame*);

// Interrupt frame structure
typedef struct interrupt_frame {
    uint64_t interrupt_number;  // Added interrupt number field
    uint64_t error_code;       // Error code (if applicable)
    uint64_t rip;              // Instruction pointer
    uint64_t cs;               // Code segment
    uint64_t rflags;           // CPU flags
    uint64_t rsp;              // Stack pointer
    uint64_t ss;               // Stack segment
} PACKED interrupt_frame_t;

// Interrupt descriptor structure
typedef struct interrupt_descriptor {
    interrupt_handler_t handler;
    bool is_registered;
    const char* description;
} interrupt_descriptor_t;

// Function declarations
void interrupt_init(void);
void register_interrupt_handler(uint8_t vector, interrupt_handler_t handler);
bool is_interrupt_registered(uint8_t vector);
const char* get_interrupt_description(uint8_t vector);
void enable_interrupts(void);
void disable_interrupts(void);

#ifdef _MSC_VER
#pragma pack(pop)
#endif

#endif // INTERRUPT_H