# Milestone 1: I/O (`kprintf`) ✅ (done)

**Goal:** formatted kernel output to both VGA and serial, so every later
milestone is debuggable.

## Concepts

C variadic functions (`stdarg.h` is freestanding-available), integer-to-string
conversion in multiple bases, layering a formatter over raw device drivers.

## Tasks

- [x] Add `src/string.c` + `include/string.h` with `memset`, `memcpy`,
      `strlen`, and integer→string helpers (`itoa`/`utoa` for base 10 and 16).
- [x] Add `src/kprintf.c` + `include/kprintf.h` implementing `kprintf(fmt, ...)`
      supporting at least: `%c %s %d %u %x %p %%`. Width/padding optional.
- [x] Route `kprintf` output to **both** VGA (`vga_putc`) and serial
      (`serial_putc`) via a small `kputc` sink.
- [x] Replace the raw `vga_puts`/`serial_puts` calls in `kernel.c` with
      `kprintf`.
- [x] Print an init line per subsystem as they come online, e.g. `[ok] serial`.

## Files

`src/string.c`, `include/string.h`, `src/kprintf.c`, `include/kprintf.h`,
edit `src/kernel.c`.

## Definition of Done

- [x] `kprintf("%s %d 0x%x\n", "n=", 42, 0xBEEF)` prints correctly on screen
      and serial.
- [x] `make iso` clean; `make run` still boots to the banner (now via
      `kprintf`).
- [x] Serial still contains `Nutshell` (CI stays green).

## References

- OSDev Wiki: [Printing to Screen](https://wiki.osdev.org/Printing_to_Screen)
- OSDev Wiki: [Meaty Skeleton](https://wiki.osdev.org/Meaty_Skeleton) (tvout/printf structure)

**Next:** [Milestone 2: GDT](milestone-2-gdt.md).
