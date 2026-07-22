#pragma once

#define GDT_KERNEL_CODE_SELECTOR 0x08
#define GDT_KERNEL_DATA_SELECTOR 0x10
#define GDT_TSS_SELECTOR 0x18
#define GDT_DOUBLE_FAULT_IST 1

void gdt_init(void);
