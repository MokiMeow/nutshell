/* kernel_main — the first C code that runs in 64-bit long mode.
 *
 * At this point boot.asm/long_mode.asm have set up paging and a 64-bit GDT.
 * There is no heap or interrupts. Formatted output is mirrored to VGA and
 * serial so every later subsystem can report its bring-up state.
 * Everything past this banner is the work laid out in docs/milestones/. */

#include "gdt.h"
#include "heap.h"
#include "idt.h"
#include "kprintf.h"
#include "keyboard.h"
#include "pic.h"
#include "pmm.h"
#include "serial.h"
#include "timer.h"
#include "vga.h"

static const char *BANNER =
    "Nutshell v0.1.0 - booted into 64-bit long mode.\n";

static void init_failure(const char *subsystem) {
    kprintf("[FAIL] %s\n", subsystem);
    __asm__ volatile ("cli");
    for (;;)
        __asm__ volatile ("hlt");
}

void kernel_main(uintptr_t multiboot_info_address) {
    struct memory_stats memory;

    gdt_init();
    serial_init();
    vga_clear();

    kprintf("[ok] gdt\n");
    kprintf("[ok] serial\n");
    kprintf("[ok] vga\n");

    if (!pmm_init(multiboot_info_address))
        init_failure("pmm");
    kprintf("[ok] pmm\n");
    if (!pmm_self_test())
        init_failure("pmm-selftest");
    kprintf("[ok] pmm-selftest\n");

    if (!heap_init())
        init_failure("heap");
    kprintf("[ok] heap\n");
    if (!heap_self_test())
        init_failure("heap-selftest");
    kprintf("[ok] heap-selftest\n");

    idt_init();
    kprintf("[ok] idt\n");
    pic_init();
    kprintf("[ok] pic\n");
    timer_init();
    kprintf("[ok] timer\n");
    keyboard_init();
    kprintf("[ok] keyboard\n");
    __asm__ volatile ("sti");
    while (timer_ticks() < TIMER_FREQUENCY_HZ)
        __asm__ volatile ("hlt");
    kprintf("[ok] timer-tick\n");

    vga_set_color(VGA_LIGHT_GREEN, VGA_BLACK);
    kprintf("%s", BANNER);
    vga_set_color(VGA_LIGHT_GREY, VGA_BLACK);
    memory = mem_stats();
    kprintf("Memory: total=%u used=%u free=%u frames, heap=%u/%u bytes\n",
            (unsigned int)memory.total_frames,
            (unsigned int)memory.used_frames,
            (unsigned int)memory.free_frames,
            (unsigned int)memory.heap_used,
            (unsigned int)memory.heap_capacity);
    kprintf("Milestone 5 complete.\n");

    for (;;)
        __asm__ volatile ("hlt");
}
