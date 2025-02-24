#ifndef INTERRUPT_H
#define INTERRUPT_H

#include <stdint.h>

// Interrupt frame for x86_64
typedef struct {
    uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
    uint64_t rdi, rsi, rbp, rsp, rbx, rdx, rcx, rax;
    uint64_t interrupt_number, error_code;
    uint64_t rip, cs, rflags, user_rsp, ss;
} interrupt_frame_t;

// Function declarations
extern void interrupt_handler(interrupt_frame_t* frame);
extern void timer_handler(interrupt_frame_t* frame);

// Get current system ticks
uint64_t get_system_ticks(void);

#endif // INTERRUPT_H