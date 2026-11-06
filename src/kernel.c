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
#ifdef KERNEL_TEST
#include "selftest.h"
#endif
#include "shell.h"
#include "timer.h"
#include "vga.h"

static const char *BANNER =
    "Nutshell v1.0.0 - booted into 64-bit long mode.\n";

static void init_failure(const char *subsystem) {
    kprintf("[FAIL] %s\n", subsystem);
    __asm__ volatile ("cli");
    for (;;)
        __asm__ volatile ("hlt");
}

void kernel_main(uintptr_t multiboot_info_address) {
#ifdef KERNEL_TEST
    uintptr_t entry_flags;
    __asm__ volatile ("pushfq; popq %0" : "=r"(entry_flags));
#endif
    gdt_init();
    serial_init();
    vga_clear();

    kprintf("[ok] gdt\n");
    kprintf("[ok] serial\n");
    kprintf("[ok] vga\n");

#ifdef KERNEL_TEST
    if ((entry_flags & (1U << 10)) != 0)
        init_failure("boot-direction-flag");
    kprintf("[ok] test-boot-df\n");
    int memory_test_line = pmm_map_self_test();
    if (memory_test_line != 0) {
        kprintf("[FAIL] test-pmm-map at line %d\n", memory_test_line);
        init_failure("pmm-map-selftest");
    }
    kprintf("[ok] test-pmm-map\n");
#endif
    /* Synthetic map tests replace allocator state; restore GRUB's map before
     * any real allocations or heap initialization. */
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
    kprintf("[ok] shell\n");
    kprintf("Milestone 6 complete.\n");
#ifdef KERNEL_TEST
    kernel_self_test();
#endif
    shell_run();
}
