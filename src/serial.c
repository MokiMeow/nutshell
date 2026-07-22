/* COM1 (0x3F8) 8250/16550 UART driver — just enough to print text.
 * QEMU exposes this as a host stream (`-serial stdio`), which is how the
 * kernel talks to your terminal and to CI before there is a screen driver. */

#include <stdint.h>
#include "serial.h"

#define COM1 0x3F8

static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t r;
    __asm__ volatile ("inb %1, %0" : "=a"(r) : "Nd"(port));
    return r;
}

void serial_init(void) {
    outb(COM1 + 1, 0x00); /* disable interrupts */
    outb(COM1 + 3, 0x80); /* enable DLAB (set baud divisor) */
    outb(COM1 + 0, 0x03); /* divisor low  = 3  -> 38400 baud */
    outb(COM1 + 1, 0x00); /* divisor high = 0 */
    outb(COM1 + 3, 0x03); /* 8 bits, no parity, one stop bit */
    outb(COM1 + 2, 0xC7); /* enable FIFO, clear, 14-byte threshold */
    outb(COM1 + 4, 0x0B); /* IRQs enabled, RTS/DSR set */
}

static int tx_ready(void) {
    return inb(COM1 + 5) & 0x20; /* line status: transmit holding empty */
}

void serial_putc(char c) {
    while (!tx_ready())
        ;
    outb(COM1, (uint8_t)c);
}

void serial_puts(const char *s) {
    for (; *s; s++) {
        if (*s == '\n')
            serial_putc('\r');
        serial_putc(*s);
    }
}
