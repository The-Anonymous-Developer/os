#include "hardware.h"

#define PIC1_COMMAND 0x20
#define PIC1_DATA    0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA    0xA1

static inline void io_wait(void) {
    #ifdef _MSC_VER
        ASM {
            xor al, al
            out 0x80, al
        }
    // #else
    //     ASM ("outb %0, $0x80" : : "a"(0));
    #endif
}

void init_pic(void) {
    // Save masks
    uint8_t mask1 = inb(PIC1_DATA);
    uint8_t mask2 = inb(PIC2_DATA);

    // Start initialization sequence
    outb(PIC1_COMMAND, 0x11);
    io_wait();
    outb(PIC2_COMMAND, 0x11);
    io_wait();

    // Set vector offsets
    outb(PIC1_DATA, 0x20);
    io_wait();
    outb(PIC2_DATA, 0x28);
    io_wait();

    // Tell PICs about each other
    outb(PIC1_DATA, 4);
    io_wait();
    outb(PIC2_DATA, 2);
    io_wait();

    // Set 8086/88 mode
    outb(PIC1_DATA, 0x01);
    io_wait();
    outb(PIC2_DATA, 0x01);
    io_wait();

    // Restore masks
    outb(PIC1_DATA, mask1);
    outb(PIC2_DATA, mask2);

    // Mask all interrupts except timer (IRQ0)
    outb(PIC1_DATA, ~0x01);  // Enable only IRQ0
    outb(PIC2_DATA, 0xFF);   // Disable all IRQs on slave PIC
}