/* kernel_main — the first C code that runs in 64-bit long mode.
 *
 * At this point boot.asm/long_mode.asm have set up paging and a 64-bit GDT.
 * There is no heap or interrupts. Formatted output is mirrored to VGA and
 * serial so every later subsystem can report its bring-up state.
 * Everything past this banner is the work laid out in docs/milestones/. */

#include "kprintf.h"
#include "serial.h"
#include "vga.h"

static const char *BANNER =
    "Nutshell v0.1.0 - booted into 64-bit long mode.\n";

void kernel_main(void) {
    serial_init();
    vga_clear();

    kprintf("[ok] serial\n");
    kprintf("[ok] vga\n");

    vga_set_color(VGA_LIGHT_GREEN, VGA_BLACK);
    kprintf("%s", BANNER);
    vga_set_color(VGA_LIGHT_GREY, VGA_BLACK);
    kprintf("%s %d 0x%x\n", "n=", 42, 0xBEEF);
    kprintf("Milestone 1 complete.\n");

    for (;;)
        __asm__ volatile ("hlt");
}
