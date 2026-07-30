# 02: Architecture

Nutshell is a single-address-space, single-privilege-level (ring 0) kernel.
There is no user space yet: the shell runs in the same space as the rest of the
kernel. That keeps the design small while still exercising all the interesting
low-level machinery.

## Layers

```
┌───────────────────────────────────────────────┐
│ shell (REPL, built-in commands)          M6    │
├───────────────────────────────────────────────┤
│ services: kmalloc, kprintf, string.c     M1,M5 │
├───────────────────────────────────────────────┤
│ drivers: keyboard, timer, serial, vga    M1,M3,M4 │
├───────────────────────────────────────────────┤
│ CPU/arch: GDT, IDT, PIC, paging          M2,M3 │
├───────────────────────────────────────────────┤
│ boot: multiboot2, 32→64-bit bring-up     M0    │
└───────────────────────────────────────────────┘
```

Lower layers never call up; upper layers depend only on the layers beneath.

## Boundaries and contracts

- **boot → kernel**: `boot/` guarantees, on entry to `kernel_main`, that the
  CPU is in 64-bit long mode with paging on (first 1 GiB identity-mapped) and a
  valid 64-bit GDT loaded. The multiboot2 info pointer (in `ebx` at boot) is
  what later gives us the memory map for milestone 5.
- **arch → drivers**: the IDT (M3) turns hardware IRQs into C handler calls.
  Drivers register/act on those. Until M3, there are no interrupts and I/O is
  polled.
- **drivers → services**: the keyboard driver fills an input ring buffer; the
  shell consumes it. The timer increments a tick counter used by `uptime`.
- **services → shell**: the shell uses `kprintf` for output and `kmalloc` for
  any dynamic memory (kept minimal).

## Memory model

- Physical: GRUB loads the kernel at **1 MiB**. Boot code identity-maps the
  first **1 GiB** with 2 MiB huge pages, so virtual == physical for everything
  the kernel touches early. (No higher-half mapping yet: a deliberate
  simplification; see [ADR 0001](decisions/0001-multiboot2-long-mode.md).)
- Dynamic: the physical allocator tracks identity-mapped frames below 1 GiB
  with allocation and ownership bitmaps. A 64 KiB first-fit, coalescing kernel
  heap (`kmalloc`/`kfree`) is backed by 16 consecutive managed frames.

## Concurrency model

Cooperative and interrupt-driven. There is no scheduler and no threads. The
main flow is:

```
kernel_main() → init subsystems → enable interrupts → shell_run()
                                                          │
        keyboard IRQ ──fills──► input ring buffer ──read──┘
        timer IRQ    ──increments──► tick counter
```

The shell polls the input buffer in a loop; interrupts do the asynchronous
work (key presses, timer). Handlers must be short and must not block.

## Source map

| Path | Layer | Added in |
|------|-------|----------|
| `boot/multiboot_header.asm` | boot | M0 |
| `boot/boot.asm`, `boot/long_mode.asm` | boot | M0 |
| `src/kernel.c` | entry | M0 |
| `src/vga.c`, `src/serial.c` | drivers | M0 |
| `src/kprintf.c` | services | M1 |
| `src/gdt.c` | arch | M2 |
| `src/idt.c`, `src/pic.c`, `src/isr.c`, `boot/isr_stubs.asm` | arch | M3 |
| `src/timer.c` | drivers | M3 |
| `src/keyboard.c` | drivers | M4 |
| `src/pmm.c`, `src/heap.c` | services | M5 |
| `src/shell.c` | shell | M6 |
| `src/string.c` | services | as needed |

See the [roadmap](04-roadmap.md) for the order and the
[milestones](milestones/) for the detail of each.
