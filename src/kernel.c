/* kernel_main — the first C code that runs in 64-bit long mode.
 *
 * At this point boot.asm/long_mode.asm have set up paging and a 64-bit GDT.
 * There is no heap, no interrupts, and no drivers beyond raw VGA + serial.
 * Everything past this banner is the work laid out in docs/milestones/. */

#include "vga.h"
#include "serial.h"

static const char *BANNER =
    "Nutshell v0.1.0 - booted into 64-bit long mode.\n";

void kernel_main(void) {
    serial_init();

    vga_clear();
    vga_set_color(VGA_LIGHT_GREEN, VGA_BLACK);
    vga_puts(BANNER);
    vga_set_color(VGA_LIGHT_GREY, VGA_BLACK);
    vga_puts("Milestone 0 complete. Next: docs/milestones/milestone-1-io.md\n");

    /* Mirror to serial so `make run` and CI can see the boot succeeded. */
    serial_puts(BANNER);
    serial_puts("Milestone 0 complete.\n");

    for (;;)
        __asm__ volatile ("hlt");
}
