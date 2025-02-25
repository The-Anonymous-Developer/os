#ifndef INTERRUPT_H
#define INTERRUPT_H

#include <stdint.h>
#include <stdbool.h>

// CPU Exception Numbers
#define INT_DIVIDE_ERROR          0
#define INT_DEBUG                 1
#define INT_NMI                  2
#define INT_BREAKPOINT           3
#define INT_OVERFLOW             4
#define INT_BOUND_RANGE          5
#define INT_INVALID_OPCODE       6
#define INT_DEVICE_NOT_AVAILABLE 7
#define INT_DOUBLE_FAULT         8
#define INT_COPROCESSOR_SEGMENT  9
#define INT_INVALID_TSS         10
#define INT_SEGMENT_NOT_PRESENT 11
#define INT_STACK_FAULT        12
#define INT_PROTECTION_FAULT   13
#define INT_PAGE_FAULT         14
#define INT_RESERVED_15        15
#define INT_FPU_ERROR         16
#define INT_ALIGNMENT_CHECK    17
#define INT_MACHINE_CHECK      18
#define INT_SIMD_EXCEPTION     19

// Hardware Interrupt Numbers
#define IRQ_BASE              32
#define IRQ_TIMER            (IRQ_BASE + 0)
#define IRQ_KEYBOARD         (IRQ_BASE + 1)
#define IRQ_CASCADE           34
#define IRQ_COM2             35
#define IRQ_COM1             36
#define IRQ_LPT2             37
#define IRQ_FLOPPY           38
#define IRQ_LPT1             39
#define IRQ_RTC              40
#define IRQ_MOUSE           (IRQ_BASE + 12)
#define IRQ_FPU              45
#define IRQ_HARDDISK        (IRQ_BASE + 14)
#define IRQ_HARDDISK2        47

// Cross-platform structure packing
#ifdef _MSC_VER
#define PACKED
#pragma pack(push, 1)
#else
#define PACKED __attribute__((packed))
#endif

// Interrupt state management
#define MAX_INTERRUPTS 256

// Forward declaration of the interrupt frame structure
struct interrupt_frame;

// Function pointer type for interrupt handlers
typedef void (*interrupt_handler_t)(struct interrupt_frame*);

// Interrupt frame structure
typedef struct {
    uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
    uint64_t rdi, rsi, rbp, rsp, rbx, rdx, rcx, rax;
    uint64_t int_no;     // Interrupt number
    uint64_t error_code; // Error code (if applicable)
    uint64_t rip, cs, rflags, user_rsp, ss;
} interrupt_frame_t;

// Interrupt descriptor structure
typedef struct interrupt_descriptor {
    interrupt_handler_t handler;
    bool is_registered;
    const char* description;
} interrupt_descriptor_t;

// Function declarations
void init_interrupt_handlers(void);
void register_interrupt_handler(uint8_t int_no, interrupt_handler_t handler);
void unregister_interrupt_handler(uint8_t int_no);
const char* get_exception_name(uint8_t int_no);
void enable_interrupts(void);
void disable_interrupts(void);
void interrupt_handler(interrupt_frame_t* frame);
void page_fault_handler(interrupt_frame_t* frame);
void double_fault_handler(interrupt_frame_t* frame);
void protection_fault_handler(interrupt_frame_t* frame);
void timer_handler(interrupt_frame_t* frame);
void keyboard_handler(uint8_t scancode);
void mouse_handler(interrupt_frame_t* frame);
void harddisk_handler(interrupt_frame_t* frame);

#ifdef _MSC_VER
#pragma pack(pop)
#endif

#endif // INTERRUPT_H