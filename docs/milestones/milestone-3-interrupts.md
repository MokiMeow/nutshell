# Milestone 3: Interrupts (IDT + PIC + PIT) ✅ (done)

**Goal:** turn hardware and CPU events into C handler calls. This unlocks the
keyboard (M4) and gives clean fault reports instead of silent reboots.

## Concepts

The IDT and interrupt-gate descriptors, CPU exceptions vs hardware IRQs,
assembly ISR stubs and the uniform interrupt frame, remapping the 8259 PIC,
the End-Of-Interrupt signal, the PIT.

## Tasks

- [x] Add `include/io.h` with `inb`/`outb` (and `io_wait`).
- [x] `boot/isr_stubs.asm`: macro-generate stubs for vectors 0–47; each pushes a
      uniform frame (vector + error code) and jumps to a common trampoline that
      calls `isr_handler` / `irq_handler`.
- [x] `src/idt.c` + `include/idt.h`: build 256 gate descriptors, install the
      stubs for 0–47, `lidt`.
- [x] `src/isr.c`: exception handler that prints vector, error code, and key
      registers via `kprintf`, then halts (for now). Install a double-fault
      handler using the IST stack from M2.
- [x] `src/pic.c`: remap master/slave PIC to vectors 32–47; mask all IRQs
      initially; helpers to unmask a line and send EOI.
- [x] `src/timer.c`: program the PIT to ~100 Hz on IRQ0; ISR increments a global
      tick counter; expose `timer_ticks()`.
- [x] Register the IRQ handlers, unmask IRQ0, and `sti` in `kernel_main`.
- [x] Print `[ok] idt`, `[ok] pic`, `[ok] timer`.

## Files

`include/io.h`, `boot/isr_stubs.asm`, `src/idt.c`, `include/idt.h`,
`src/isr.c`, `src/pic.c`, `include/pic.h`, `src/timer.c`, `include/timer.h`,
edit `src/kernel.c`.

## Definition of Done

- [x] After `sti`, the kernel keeps running (no triple fault) and the tick
      counter increases over time (log it, or busy-print every N ticks).
- [x] Deliberately dividing by zero (temporary test) prints a fault report
      instead of rebooting.
- [x] `make iso` clean, `make run` boots, CI green.

## References

- OSDev Wiki: [Interrupt Descriptor Table](https://wiki.osdev.org/Interrupt_Descriptor_Table)
- OSDev Wiki: [8259 PIC](https://wiki.osdev.org/8259_PIC)
- OSDev Wiki: [Programmable Interval Timer](https://wiki.osdev.org/Programmable_Interval_Timer)
- OSDev Wiki: [Interrupts Tutorial](https://wiki.osdev.org/Interrupts_Tutorial)

**Next:** [Milestone 4: Keyboard](milestone-4-keyboard.md).
