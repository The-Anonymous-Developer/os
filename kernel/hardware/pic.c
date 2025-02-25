#include "pic.h"
#include "ports.h"
#include "cpu.h"

#define PIC1_COMMAND 0x20
#define PIC1_DATA    0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA    0xA1

#define PIC1_OFFSET 0x20
#define PIC2_OFFSET 0x28

static uint16_t pic1_mask = 0xFFFF;
static uint16_t pic2_mask = 0xFFFF;

void init_pic(void) {
    // Save masks
    uint8_t mask1 = inb(PIC1_DATA);
    uint8_t mask2 = inb(PIC2_DATA);

    // Initialize both PICs
    outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait();  // Using io_wait from ports.h
    outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait();

    // Set vector offsets
    outb(PIC1_DATA, PIC1_OFFSET);
    io_wait();
    outb(PIC2_DATA, PIC2_OFFSET);
    io_wait();

    // Tell Master PIC there is a slave
    outb(PIC1_DATA, 4);
    io_wait();
    outb(PIC2_DATA, 2);
    io_wait();

    // Set 8086 mode
    outb(PIC1_DATA, ICW4_8086);
    io_wait();
    outb(PIC2_DATA, ICW4_8086);
    io_wait();

    // Restore saved masks
    outb(PIC1_DATA, mask1);
    outb(PIC2_DATA, mask2);
}

void pic_mask_irq(uint8_t irq) {
    uint16_t port;
    uint8_t value;

    if (irq < 8) {
        port = PIC1_DATA;
        value = inb(port) | (1 << irq);
        pic1_mask = value;
    } else {
        port = PIC2_DATA;
        value = inb(port) | (1 << (irq - 8));
        pic2_mask = value;
    }
    outb(port, value);
}

void pic_unmask_irq(uint8_t irq) {
    uint16_t port;
    uint8_t value;

    if (irq < 8) {
        port = PIC1_DATA;
        value = inb(port) & ~(1 << irq);
        pic1_mask = value;
    } else {
        port = PIC2_DATA;
        value = inb(port) & ~(1 << (irq - 8));
        pic2_mask = value;
    }
    outb(port, value);
}

void pic_send_eoi(uint8_t irq) {
    if (irq >= 8)
        outb(PIC2_COMMAND, 0x20);
    outb(PIC1_COMMAND, 0x20);
}