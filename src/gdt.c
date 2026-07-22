#include <stddef.h>
#include <stdint.h>

#include "gdt.h"
#include "string.h"

#define GDT_ENTRY_COUNT 5
#define GDT_CODE_DESCRIPTOR 0x00AF9A000000FFFFULL
#define GDT_DATA_DESCRIPTOR 0x00CF92000000FFFFULL
#define TSS_ACCESS_PRESENT_AVAILABLE 0x89
#define KERNEL_STACK_SIZE 16384
#define DOUBLE_FAULT_STACK_SIZE 4096

struct gdt_pointer {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));

struct tss {
    uint32_t reserved0;
    uint64_t rsp0;
    uint64_t rsp1;
    uint64_t rsp2;
    uint64_t reserved1;
    uint64_t ist1;
    uint64_t ist2;
    uint64_t ist3;
    uint64_t ist4;
    uint64_t ist5;
    uint64_t ist6;
    uint64_t ist7;
    uint64_t reserved2;
    uint16_t reserved3;
    uint16_t iomap_base;
} __attribute__((packed));

static uint64_t gdt[GDT_ENTRY_COUNT] __attribute__((aligned(16)));
static struct tss kernel_tss;
static uint8_t kernel_stack[KERNEL_STACK_SIZE] __attribute__((aligned(16)));
static uint8_t double_fault_stack[DOUBLE_FAULT_STACK_SIZE]
    __attribute__((aligned(16)));

static void set_tss_descriptor(uintptr_t base, uint32_t limit) {
    gdt[3] = (uint64_t)(limit & 0xFFFF)
        | ((uint64_t)(base & 0xFFFFFF) << 16)
        | ((uint64_t)TSS_ACCESS_PRESENT_AVAILABLE << 40)
        | ((uint64_t)((limit >> 16) & 0x0F) << 48)
        | ((uint64_t)((base >> 24) & 0xFF) << 56);
    gdt[4] = (uint64_t)(base >> 32);
}

static void load_gdt(const struct gdt_pointer *pointer) {
    __asm__ volatile (
        "lgdt %0\n"
        "movw %2, %%ax\n"
        "movw %%ax, %%ds\n"
        "movw %%ax, %%es\n"
        "movw %%ax, %%ss\n"
        "movw %%ax, %%fs\n"
        "movw %%ax, %%gs\n"
        "pushq %1\n"
        "leaq 1f(%%rip), %%rax\n"
        "pushq %%rax\n"
        "lretq\n"
        "1:\n"
        "movw %3, %%ax\n"
        "ltr %%ax"
        :
        : "m"(*pointer), "i"(GDT_KERNEL_CODE_SELECTOR),
          "i"(GDT_KERNEL_DATA_SELECTOR), "i"(GDT_TSS_SELECTOR)
        : "rax", "memory"
    );
}

void gdt_init(void) {
    struct gdt_pointer pointer;

    memset(gdt, 0, sizeof(gdt));
    memset(&kernel_tss, 0, sizeof(kernel_tss));

    kernel_tss.rsp0 = (uintptr_t)(kernel_stack + sizeof(kernel_stack));
    kernel_tss.ist1 = (uintptr_t)(double_fault_stack
                                  + sizeof(double_fault_stack));
    kernel_tss.iomap_base = sizeof(kernel_tss);

    gdt[1] = GDT_CODE_DESCRIPTOR;
    gdt[2] = GDT_DATA_DESCRIPTOR;
    set_tss_descriptor((uintptr_t)&kernel_tss, sizeof(kernel_tss) - 1);

    pointer.limit = sizeof(gdt) - 1;
    pointer.base = (uintptr_t)gdt;
    load_gdt(&pointer);
}
