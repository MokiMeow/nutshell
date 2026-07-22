#include <stddef.h>
#include <stdint.h>

#include "gdt.h"
#include "idt.h"
#include "string.h"

#define IDT_ENTRY_COUNT 256
#define INSTALLED_VECTOR_COUNT 48
#define INTERRUPT_GATE_PRESENT 0x8E
#define DOUBLE_FAULT_VECTOR 8

struct idt_entry {
    uint16_t offset_low;
    uint16_t selector;
    uint8_t ist;
    uint8_t type_attributes;
    uint16_t offset_middle;
    uint32_t offset_high;
    uint32_t reserved;
} __attribute__((packed));

struct idt_pointer {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));

extern void *isr_stub_table[];

static struct idt_entry idt[IDT_ENTRY_COUNT] __attribute__((aligned(16)));

static void idt_set_gate(uint8_t vector, uintptr_t handler, uint8_t ist) {
    struct idt_entry *entry = &idt[vector];

    entry->offset_low = handler & 0xFFFF;
    entry->selector = GDT_KERNEL_CODE_SELECTOR;
    entry->ist = ist & 0x07;
    entry->type_attributes = INTERRUPT_GATE_PRESENT;
    entry->offset_middle = (handler >> 16) & 0xFFFF;
    entry->offset_high = (handler >> 32) & 0xFFFFFFFF;
    entry->reserved = 0;
}

void idt_init(void) {
    struct idt_pointer pointer;

    memset(idt, 0, sizeof(idt));
    for (uint8_t vector = 0; vector < INSTALLED_VECTOR_COUNT; vector++) {
        uint8_t ist = vector == DOUBLE_FAULT_VECTOR
            ? GDT_DOUBLE_FAULT_IST : 0;
        idt_set_gate(vector, (uintptr_t)isr_stub_table[vector], ist);
    }

    pointer.limit = sizeof(idt) - 1;
    pointer.base = (uintptr_t)idt;
    __asm__ volatile ("lidt %0" : : "m"(pointer) : "memory");
}
