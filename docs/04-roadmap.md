# 04 — Roadmap

The path from "boots to a banner" (today) to "boots into a working shell." Each
milestone is a self-contained step that leaves a bootable kernel behind, with a
full spec and a Definition of Done in [milestones/](milestones/).

This is designed as **one focused week of work**, one milestone per day-ish,
with a polish day at the end. Move at your own pace — the ordering is what
matters, not the calendar.

## The plan

| Day | # | Milestone | You'll build | You'll learn |
|-----|---|-----------|--------------|--------------|
| — | 0 | **Boot** ✅ | multiboot2, 32→64-bit, paging, VGA+serial banner | protected vs long mode, page tables, GDT |
| 1 | 1 | **I/O** ✅ | `kprintf` (formatted output to VGA + serial) | varargs, number formatting, driver layering |
| 1 | 2 | **GDT** ✅ | a proper 64-bit GDT (+ TSS stub) in C | segmentation, descriptors, `lgdt` |
| 2–3 | 3 | **Interrupts** ✅ | IDT, remap the 8259 PIC, exceptions, IRQs, PIT timer | interrupt vectors, IRQ handling, EOI |
| 4 | 4 | **Keyboard** ✅ | PS/2 IRQ1 driver, scancode decode, input ring buffer | device I/O, scancode set 1, buffering |
| 5 | 5 | **Memory** | physical frame allocator + kernel heap (`kmalloc`) | the multiboot memory map, allocators |
| 6 | 6 | **Shell** | REPL + built-ins | parsing, command dispatch, putting it together |
| 7 | 7 | **Polish** | tests, CI, README GIF, tag `v1.0.0` | QEMU test harness, presentation |

## Dependency order

```
M0 ─► M1 ─► M2 ─► M3 ─► M4 ─┐
                  └► M5 ────┴─► M6 ─► M7
```

- **M1 before everything** — you want `kprintf` to debug the rest.
- **M2 before M3** — the IDT needs a valid code segment selector from the GDT.
- **M3 before M4** — the keyboard is interrupt-driven (IRQ1).
- **M5 can be built in parallel with M3/M4** — it only depends on M1 and the
  multiboot info pointer saved at boot.
- **M6 needs M4 (input) and benefits from M5 (`kmalloc`)**.

## Definition of Done (whole project)

Nutshell boots in QEMU to an interactive prompt. The user types `help`, `echo
hello`, `mem`, `uptime`, `clear`, `reboot`; each works. Everything runs on the
bare kernel with no host OS. CI is green, the README has the boot-to-shell GIF,
and the release is tagged `v1.0.0`.

## Stretch goals (after v1.0.0 — optional, not required)

- Command history and line editing (arrow keys, backspace already in M4).
- A tiny in-RAM filesystem with `ls` / `cat` over baked-in files.
- Higher-half kernel mapping.
- `x86_64-elf` cross-compiler build in CI for portability.
- A second architecture check or ACPI-based reboot/poweroff.

Keep these clearly marked as extras; **v1.0.0 is the shell.** Don't let the
stretch list delay shipping.
