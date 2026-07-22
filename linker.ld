/* Nutshell linker script.
 *
 * The kernel is loaded by GRUB at the 1 MiB physical mark (below that is
 * reserved for legacy BIOS / VGA memory). The multiboot2 header MUST come
 * first in the binary so GRUB can find it within the first 32 KiB. */

ENTRY(start)

SECTIONS {
    . = 1M;

    .boot :
    {
        /* KEEP so the header is never garbage-collected out of the binary. */
        KEEP(*(.multiboot_header))
    }

    .text :
    {
        *(.text)
    }

    .rodata :
    {
        *(.rodata)
    }

    .data :
    {
        *(.data)
    }

    .bss :
    {
        *(.bss)
    }
}
