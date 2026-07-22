#pragma once

#include <stdint.h>

#define IRQ_COUNT 16

struct interrupt_frame {
    uint64_t r15;
    uint64_t r14;
    uint64_t r13;
    uint64_t r12;
    uint64_t r11;
    uint64_t r10;
    uint64_t r9;
    uint64_t r8;
    uint64_t rdi;
    uint64_t rsi;
    uint64_t rbp;
    uint64_t rdx;
    uint64_t rcx;
    uint64_t rbx;
    uint64_t rax;
    uint64_t vector;
    uint64_t error_code;
    uint64_t rip;
    uint64_t cs;
    uint64_t rflags;
};

typedef void (*irq_callback_t)(struct interrupt_frame *frame);

void isr_handler(struct interrupt_frame *frame);
void irq_handler(struct interrupt_frame *frame);
void irq_register_handler(uint8_t irq, irq_callback_t callback);
void isr_trigger_divide_error(void);
