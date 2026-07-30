# Milestone 0: Boot ✅ (done)

**Goal:** boot on a bare x86-64 machine, reach 64-bit long mode, and print a
banner to screen and serial.

## Concepts

Multiboot2, protected vs long mode, 4-level paging, PAE, `EFER.LME`, the GDT,
the far jump that switches to 64-bit, VGA text mode, COM1 serial.

## What shipped

- [x] Multiboot2 header (`boot/multiboot_header.asm`).
- [x] 32-bit bring-up (`boot/boot.asm`): multiboot/CPUID/long-mode checks,
      identity-map the first 1 GiB with 2 MiB pages, enable PAE + long mode +
      paging, load a 64-bit GDT, far-jump to 64-bit.
- [x] 64-bit entry (`boot/long_mode.asm`) → `kernel_main`.
- [x] VGA text driver (`src/vga.c`) and COM1 serial driver (`src/serial.c`).
- [x] `kernel_main` banner to VGA + serial (`src/kernel.c`).
- [x] Build system: `Makefile`, `linker.ld`, `grub/grub.cfg`.
- [x] CI smoke-boot that greps serial for the banner.

## Definition of Done

- [x] `make clean && make iso` builds with no warnings.
- [x] `make run` boots in QEMU and prints the banner on screen.
- [x] The serial log contains `Nutshell`.

## References

- OSDev Wiki: [Setting Up Long Mode](https://wiki.osdev.org/Setting_Up_Long_Mode)
- Philipp Oppermann: [A minimal x86 kernel](https://os.phil-opp.com/multiboot-kernel/),
  [Entering Long Mode](https://os.phil-opp.com/entering-longmode/)

**Next:** [Milestone 1: I/O](milestone-1-io.md).
