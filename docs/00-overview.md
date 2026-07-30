# 00: Overview

## What Nutshell is

Nutshell is a 64-bit x86 operating-system kernel written from scratch in C and
assembly. It boots on a bare machine (via GRUB), brings the CPU up into 64-bit
long mode itself, installs its own interrupt handling, drives the keyboard,
manages physical memory, and boots into an interactive shell: with no
operating system beneath it, because it *is* the operating system.

## Design goals

1. **Understandable end to end.** One person should be able to read every line
   and know why it exists. Clarity beats cleverness.
2. **From scratch, but not reinventing the boot chain.** We use GRUB as the
   bootloader (via the multiboot2 standard) so we can focus on the *kernel*;
   the CPU bring-up, interrupts, memory, drivers, and shell are all ours.
3. **Always bootable.** Every commit builds and boots. The project grows by
   milestones, each of which leaves a working kernel behind.
4. **Zero cost, zero accounts.** Everything runs locally on WSL2/Linux with
   free tools (nasm, gcc, GRUB, QEMU). No cloud, no license keys.

## What it is *not*

- Not a full OS: no user space, no processes, no networking, no persistent
  filesystem (a tiny in-memory one is a stretch goal, not a requirement).
- Not POSIX. The shell is a small custom REPL, not bash.
- Not a bootloader project: GRUB handles the firmware handshake.

## Scope, in one sentence

> Boot a bare x86-64 machine into 64-bit long mode and land the user at an
> interactive prompt, built entirely from our own kernel code.

## The shape of the system

```
firmware (BIOS)
    │
    ▼
GRUB  ──multiboot2──►  boot/  (32-bit protected mode)
                         │  CPUID checks, page tables,
                         │  PAE + long mode + paging, 64-bit GDT
                         ▼
                       kernel  (64-bit long mode, C)
                         ├─ VGA text + serial output   (M0, M1)
                         ├─ GDT / TSS                   (M2)
                         ├─ IDT + PIC + PIT             (M3)
                         ├─ PS/2 keyboard driver        (M4)
                         ├─ physical memory + heap      (M5)
                         └─ shell (REPL + built-ins)    (M6)
```

Read the [architecture doc](02-architecture.md) next for how these layers talk
to each other, or jump straight to [getting started](01-getting-started.md) to
build and boot it.
