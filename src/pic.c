#include <stdint.h>

#include "io.h"
#include "pic.h"

#define PIC_MASTER_COMMAND 0x20
#define PIC_MASTER_DATA 0x21
#define PIC_SLAVE_COMMAND 0xA0
#define PIC_SLAVE_DATA 0xA1
#define PIC_EOI 0x20
#define PIC_ICW1_INIT 0x10
#define PIC_ICW1_ICW4 0x01
#define PIC_ICW4_8086 0x01
#define PIC_MASTER_OFFSET 32
#define PIC_SLAVE_OFFSET 40
#define PIC_MASTER_CASCADE_IRQ 2
#define PIC_SLAVE_IDENTITY 2
#define PIC_ALL_MASKED 0xFF

void pic_init(void) {
    outb(PIC_MASTER_COMMAND, PIC_ICW1_INIT | PIC_ICW1_ICW4);
    io_wait();
    outb(PIC_SLAVE_COMMAND, PIC_ICW1_INIT | PIC_ICW1_ICW4);
    io_wait();

    outb(PIC_MASTER_DATA, PIC_MASTER_OFFSET);
    io_wait();
    outb(PIC_SLAVE_DATA, PIC_SLAVE_OFFSET);
    io_wait();

    outb(PIC_MASTER_DATA, 1 << PIC_MASTER_CASCADE_IRQ);
    io_wait();
    outb(PIC_SLAVE_DATA, PIC_SLAVE_IDENTITY);
    io_wait();

    outb(PIC_MASTER_DATA, PIC_ICW4_8086);
    io_wait();
    outb(PIC_SLAVE_DATA, PIC_ICW4_8086);
    io_wait();

    outb(PIC_MASTER_DATA, PIC_ALL_MASKED);
    outb(PIC_SLAVE_DATA, PIC_ALL_MASKED);
}

void pic_unmask(uint8_t irq) {
    uint16_t port;
    uint8_t line;
    uint8_t mask;

    if (irq < 8) {
        port = PIC_MASTER_DATA;
        line = irq;
    } else if (irq < 16) {
        port = PIC_SLAVE_DATA;
        line = irq - 8;
        pic_unmask(PIC_MASTER_CASCADE_IRQ);
    } else {
        return;
    }

    mask = inb(port) & (uint8_t)~(1U << line);
    outb(port, mask);
}

void pic_send_eoi(uint8_t irq) {
    if (irq >= 8)
        outb(PIC_SLAVE_COMMAND, PIC_EOI);
    outb(PIC_MASTER_COMMAND, PIC_EOI);
}
