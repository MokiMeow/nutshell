# Milestone 4 — Keyboard ✅ (done)

**Goal:** an interrupt-driven PS/2 keyboard driver that turns key presses into
characters in a ring buffer the shell can read.

## Concepts

Device I/O via the 8042 controller, IRQ1, scancode set 1 (make/break codes),
modifier state, the producer/consumer ring buffer pattern for ISR-safe input.

## Tasks

- [x] `src/ringbuf.c` + `include/ringbuf.h`: a fixed-size circular byte buffer
      with `push`/`pop`/`empty` (single-producer ISR, single-consumer loop).
- [x] `src/keyboard.c` + `include/keyboard.h`: IRQ1 handler that reads port
      `0x60`, decodes scancode set 1 to ASCII with a lookup table, tracks Shift,
      handles Enter/Backspace, and pushes characters to the ring buffer.
- [x] Register the handler on IRQ1 and unmask it in the PIC.
- [x] Expose `keyboard_getchar()` (blocking: `hlt` until the buffer is
      non-empty) for the shell.
- [x] Temporary test: echo typed characters in `kernel_main`.
- [x] Print `[ok] keyboard`.

## Files

`src/ringbuf.c`, `include/ringbuf.h`, `src/keyboard.c`, `include/keyboard.h`,
edit `src/kernel.c` and the IRQ registration from M3.

## Definition of Done

- [x] Typing in QEMU echoes characters to the screen in real time.
- [x] Shift produces upper case / symbols; Backspace erases; Enter yields `\n`.
- [x] No missed keys under normal typing (ISR stays short; EOI sent).
- [x] `make iso` clean, `make run` boots, CI green.

## References

- OSDev Wiki — [PS/2 Keyboard](https://wiki.osdev.org/PS/2_Keyboard)
- OSDev Wiki — [PS/2 Controller](https://wiki.osdev.org/%228042%22_PS/2_Controller)

**Next:** [Milestone 5 — Memory](milestone-5-memory.md).
