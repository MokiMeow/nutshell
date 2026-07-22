#include <stddef.h>
#include <stdint.h>

#include "isr.h"
#include "kprintf.h"
#include "pic.h"

#define IRQ_VECTOR_BASE 32

static irq_callback_t irq_callbacks[IRQ_COUNT];

void irq_register_handler(uint8_t irq, irq_callback_t callback) {
    if (irq < IRQ_COUNT)
        irq_callbacks[irq] = callback;
}

void isr_handler(struct interrupt_frame *frame) {
    __asm__ volatile ("cli");
    kprintf("\n[FAULT] exception %u error=0x%x\n",
            (unsigned int)frame->vector, (unsigned int)frame->error_code);
    kprintf("rip=%p cs=0x%x rflags=0x%x\n", (void *)frame->rip,
            (unsigned int)frame->cs, (unsigned int)frame->rflags);
    kprintf("rax=%p rbx=%p rcx=%p rdx=%p\n", (void *)frame->rax,
            (void *)frame->rbx, (void *)frame->rcx, (void *)frame->rdx);

    for (;;)
        __asm__ volatile ("hlt");
}

void irq_handler(struct interrupt_frame *frame) {
    uint8_t irq = frame->vector - IRQ_VECTOR_BASE;

    if (irq < IRQ_COUNT && irq_callbacks[irq] != NULL)
        irq_callbacks[irq](frame);

    if (irq < IRQ_COUNT)
        pic_send_eoi(irq);
}

void isr_trigger_divide_error(void) {
    __asm__ volatile (
        "xor %%rdx, %%rdx\n"
        "mov $1, %%rax\n"
        "xor %%rcx, %%rcx\n"
        "div %%rcx"
        :
        :
        : "rax", "rcx", "rdx"
    );
}
