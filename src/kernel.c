/* kernel_main — the first C code that runs in 64-bit long mode.
 *
 * At this point boot.asm/long_mode.asm have set up paging and a 64-bit GDT.
 * There is no heap or interrupts. Formatted output is mirrored to VGA and
 * serial so every later subsystem can report its bring-up state.
 * Everything past this banner is the work laid out in docs/milestones/. */

#include "gdt.h"
#include "idt.h"
#include "kprintf.h"
#include "keyboard.h"
#include "pic.h"
#include "serial.h"
#include "timer.h"
#include "vga.h"

static const char *BANNER =
    "Nutshell v0.1.0 - booted into 64-bit long mode.\n";

void kernel_main(void) {
    gdt_init();
    serial_init();
    vga_clear();

    kprintf("[ok] gdt\n");
    kprintf("[ok] serial\n");
    kprintf("[ok] vga\n");

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
    kprintf("Milestone 4 complete. Keyboard echo test:\n");

    for (;;)
        kprintf("%c", keyboard_getchar());
}
