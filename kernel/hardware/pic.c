#include "pic.h"
#include "ports.h"
#include "cpu.h"

#define PIC1_COMMAND 0x20
#define PIC1_DATA    0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA    0xA1

#define PIC1_OFFSET 0x20
#define PIC2_OFFSET 0x28

#define PIC_EOI 0x20

static uint16_t pic1_mask = 0xFFFF;
static uint16_t pic2_mask = 0xFFFF;

void init_pic(void) {
    pic_remap(PIC1_OFFSET, PIC2_OFFSET);
}

void pic_remap(uint8_t offset1, uint8_t offset2) {
    uint8_t mask1 = inb(PIC1_DATA);    // Save masks
    uint8_t mask2 = inb(PIC2_DATA);

    outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);  // Start initialization
    outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
    
    outb(PIC1_DATA, offset1);     // ICW2: Master PIC vector offset
    outb(PIC2_DATA, offset2);     // ICW2: Slave PIC vector offset
    
    outb(PIC1_DATA, 4);           // ICW3: tell Master PIC there is a slave at IRQ2
    outb(PIC2_DATA, 2);           // ICW3: tell Slave PIC its cascade identity
    
    outb(PIC1_DATA, ICW4_8086);   // ICW4: have the PICs use 8086 mode
    outb(PIC2_DATA, ICW4_8086);

    outb(PIC1_DATA, mask1);       // Restore saved masks
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
    if (irq >= 8) {
        outb(PIC2_COMMAND, PIC_EOI);
    }
    outb(PIC1_COMMAND, PIC_EOI);
}

void pic_disable(void) {
    outb(PIC1_DATA, 0xFF);
    outb(PIC2_DATA, 0xFF);
}