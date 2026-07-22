#include <stdint.h>

#include "io.h"
#include "isr.h"
#include "pic.h"
#include "timer.h"

#define PIT_CHANNEL0_DATA 0x40
#define PIT_COMMAND 0x43
#define PIT_INPUT_FREQUENCY 1193182
#define PIT_CHANNEL0_LOHI_MODE3 0x36
#define TIMER_IRQ 0

static volatile uint64_t ticks;

static void timer_interrupt(struct interrupt_frame *frame) {
    (void)frame;
    ticks++;
}

void timer_init(void) {
    uint16_t divisor = PIT_INPUT_FREQUENCY / TIMER_FREQUENCY_HZ;

    ticks = 0;
    irq_register_handler(TIMER_IRQ, timer_interrupt);
    outb(PIT_COMMAND, PIT_CHANNEL0_LOHI_MODE3);
    outb(PIT_CHANNEL0_DATA, divisor & 0xFF);
    outb(PIT_CHANNEL0_DATA, divisor >> 8);
    pic_unmask(TIMER_IRQ);
}

uint64_t timer_ticks(void) {
    return ticks;
}
