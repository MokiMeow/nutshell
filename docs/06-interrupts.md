# 06 — Interrupts

*Implemented in milestone 3.* Concept reference; build steps in
[milestones/milestone-3-interrupts.md](milestones/milestone-3-interrupts.md).

## Why interrupts

Without interrupts, the only way to notice a key press or a timer tick is to
poll. Interrupts let the hardware call *us*: the CPU stops what it's doing, runs
a handler, and resumes. This is what makes the keyboard (and therefore the
shell) event-driven.

## The Interrupt Descriptor Table (IDT)

The IDT maps each of the 256 interrupt vectors to a handler address, with type
and privilege bits. We build one with 64-bit interrupt-gate descriptors and
load it with `lidt`. Vectors split into:

- **0–31**: CPU **exceptions** (divide-by-zero, page fault, GPF, double
  fault, …). We install handlers that at minimum print the vector + error code
  and halt — invaluable for debugging.
- **32–47**: hardware **IRQs**, after we remap the PIC (see below).
- the rest: unused for now.

## The assembly stubs

Each vector needs a tiny assembly stub (`boot/isr_stubs.asm`) because the CPU
pushes different frames for exceptions with vs without an error code. Each stub
pushes a uniform frame (vector number, dummy error code where needed) and jumps
to a common handler that saves every general-purpose register, aligns the
stack, and calls into C (`src/isr.c`). Macros generate the 48 stubs. Vector 8
uses the TSS's dedicated IST1 stack so a damaged normal stack cannot prevent a
double-fault report.

## Remapping the 8259 PIC

The legacy Programmable Interrupt Controller delivers IRQs 0–15, but by default
on vectors that collide with CPU exceptions. We reprogram (remap) the master
and slave PICs so hardware IRQs arrive on vectors **32–47** instead. After
handling an IRQ we must send an **End Of Interrupt (EOI)** or that line never
fires again.

Key IRQs we care about:
- **IRQ0** — PIT timer.
- **IRQ1** — PS/2 keyboard (milestone 4).

## The PIT timer

The Programmable Interval Timer fires IRQ0 at a rate we set (e.g. 100 Hz). We
count ticks; `uptime` in the shell reports them. It's also the first proof the
whole interrupt path works end to end. During boot, the kernel sleeps until
100 ticks have arrived and then prints `[ok] timer-tick` synchronously.

## Sequence, once wired up

```
device raises IRQ ─► PIC ─► CPU ─► IDT[vector] ─► asm stub ─► isr.c handler
                                                                  │
                                                       do work, send EOI
                                                                  ▼
                                                          CPU resumes
```

## What to demonstrate

- Deliberately trigger an exception (e.g. divide by zero) and show the handler
  printing a clean fault report instead of a silent triple-fault reboot.
- A timer tick counter incrementing, visible via `uptime`.

## References

- OSDev Wiki — [Interrupt Descriptor Table](https://wiki.osdev.org/Interrupt_Descriptor_Table)
- OSDev Wiki — [8259 PIC](https://wiki.osdev.org/8259_PIC)
- OSDev Wiki — [Programmable Interval Timer](https://wiki.osdev.org/Programmable_Interval_Timer)
