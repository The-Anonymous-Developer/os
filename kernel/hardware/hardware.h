#ifndef HARDWARE_H
#define HARDWARE_H

#include <stdint.h>

// Cross-platform assembly support
#ifdef _MSC_VER
    #define ASM __asm
#else
    #define ASM __asm__
#endif

// GDT structure definitions
#pragma pack(push, 1)
typedef struct {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_middle;
    uint8_t access;
    uint8_t granularity;
    uint8_t base_high;
} gdt_entry_t;

typedef struct {
    uint16_t limit;
    uint64_t base;
} gdt_ptr_t;

// IDT structure definitions
typedef struct {
    uint16_t base_low;
    uint16_t selector;
    uint8_t ist;
    uint8_t flags;
    uint16_t base_middle;
    uint32_t base_high;
    uint32_t reserved;
} idt_entry_t;

typedef struct {
    uint16_t limit;
    uint64_t base;
} idt_ptr_t;
#pragma pack(pop)

// I/O functions
static inline uint8_t inb(uint16_t port) {
    uint8_t ret = 0;
    #ifdef _MSC_VER
        ASM {
            mov dx, port
            in al, dx
            mov ret, al
        }
    // #else
    //     ASM (
    //         "inb %1, %0"
    //         : "=a"(ret)
    //         : "Nd"(port)
    //     );
    #endif
    return ret;
}

static inline void outb(uint16_t port, uint8_t val) {
    #ifdef _MSC_VER
        ASM {
            mov dx, port
            mov al, val
            out dx, al
        }
    // #else
    //     ASM (
    //         "outb %0, %1"
    //         : 
    //         : "a"(val), "Nd"(port)
    //     );
    #endif
}

// PIT settings
#define PIT_FREQUENCY   1193182
#define TIMER_FREQ      100

// Function declarations
extern void init_hardware(void);
extern void init_gdt(void);
extern void init_idt(void);
extern void init_pic(void);
extern void init_pit(void);

#endif // HARDWARE_H