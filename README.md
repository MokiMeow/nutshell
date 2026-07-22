<h1 align="center">Nutshell</h1>

<p align="center">
  <em>A 64-bit x86 operating-system kernel, written from scratch in C and assembly,
  that boots into its own interactive shell.</em>
</p>

<p align="center">
  <img src="https://img.shields.io/badge/arch-x86__64-blue" alt="x86_64">
  <img src="https://img.shields.io/badge/lang-C%20%2B%20Assembly-orange" alt="C + Assembly">
  <img src="https://img.shields.io/badge/boot-Multiboot2%20%2F%20GRUB-green" alt="Multiboot2">
  <img src="https://img.shields.io/badge/license-MIT-lightgrey" alt="MIT">
</p>

---

## What this is

Nutshell is a small operating-system kernel built the hard way: no libc, no
framework, no training wheels. GRUB drops us into 32-bit protected mode; from
there the kernel sets up its own page tables, enters 64-bit long mode, installs
interrupt handlers, drives the keyboard, manages physical memory, and finally
boots into a shell running *on the bare metal* — no OS underneath it, because
this **is** the OS.

It is deliberately scoped: enough to be real, small enough for one person to
understand end to end.

```
+-----------+   GRUB    +------------------+  long mode  +----------------+
|  BIOS /   | --------> |  boot.asm (32b)  | ----------> |  kernel (C,64b)|
|  GRUB     | multiboot2|  paging + GDT +  |  far jump   |  VGA, IDT, PIC |
+-----------+           |  long-mode enter |             |  keyboard, mem |
                        +------------------+             |  -> shell      |
                                                         +----------------+
```

## Why it is interesting (the depth on show)

- **The 32 → 64-bit transition, by hand** — CPUID feature detection, building
  4-level page tables, enabling PAE + `EFER.LME` + paging, and far-jumping
  through a 64-bit GDT. ([docs/03-boot-process.md](docs/03-boot-process.md))
- **Interrupts from the silicon up** — a 64-bit IDT, remapping the legacy 8259
  PIC, exception and IRQ handlers, and the PIT timer.
  ([docs/06-interrupts.md](docs/06-interrupts.md))
- **Your own memory manager** — a physical-frame allocator over the multiboot
  memory map and a kernel heap, so `malloc` means *your* `malloc`.
  ([docs/05-memory-management.md](docs/05-memory-management.md))
- **A real driver** — PS/2 keyboard on IRQ1, scancode-set-1 decoding, and an
  input ring buffer feeding the shell.
  ([docs/07-keyboard-and-drivers.md](docs/07-keyboard-and-drivers.md))
- **A shell on bare metal** — a REPL with built-in commands, running with
  nothing beneath it. ([docs/08-the-shell.md](docs/08-the-shell.md))

## Quick start (WSL2 / Linux, $0, no accounts)

```bash
./scripts/setup-wsl.sh   # one-time: nasm, gcc, binutils, grub, qemu
make run                 # build the ISO and boot Nutshell in QEMU
```

You should see Nutshell boot into 64-bit long mode and print its banner.
Serial output is mirrored to your terminal. See
[docs/01-getting-started.md](docs/01-getting-started.md) for details, and
[docs/09-testing-and-debugging.md](docs/09-testing-and-debugging.md) for the
QEMU + GDB workflow.

## Status

Milestone 0 (boot into 64-bit long mode) is **done** — the repo builds and
boots today. The road from here to a working shell is tracked, milestone by
milestone, in [docs/04-roadmap.md](docs/04-roadmap.md).

| # | Milestone | State |
|---|-----------|-------|
| 0 | Boot to 64-bit long mode | ✅ done |
| 1 | Screen + serial I/O (`kprintf`) | ⬜ |
| 2 | Global Descriptor Table | ⬜ |
| 3 | Interrupts (IDT + PIC + timer) | ⬜ |
| 4 | PS/2 keyboard driver | ⬜ |
| 5 | Physical memory + kernel heap | ⬜ |
| 6 | The shell | ⬜ |
| 7 | Polish, tests, portfolio pass | ⬜ |

## Repository layout

```
nutshell/
├── boot/         # assembly: multiboot header, 32->64-bit bring-up
├── src/          # C kernel sources (auto-discovered by the Makefile)
├── include/      # kernel headers
├── grub/         # GRUB config baked into the ISO
├── scripts/      # toolchain setup and helpers
├── docs/         # architecture, boot process, roadmap, milestones, ADRs
├── linker.ld     # places the multiboot header first; loads kernel at 1 MiB
└── Makefile      # build / iso / run / debug / clean
```

## Documentation

Everything is in [`docs/`](docs/). Start with
[docs/00-overview.md](docs/00-overview.md), or jump to the
[roadmap](docs/04-roadmap.md) to see how each piece gets built.

## License

MIT — see [LICENSE](LICENSE).
