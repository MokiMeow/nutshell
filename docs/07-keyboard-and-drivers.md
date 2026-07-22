# 07 — Keyboard & drivers

*Implemented in milestone 4.* Concept reference; build steps in
[milestones/milestone-4-keyboard.md](milestones/milestone-4-keyboard.md).

## The PS/2 keyboard

QEMU emulates a legacy PS/2 keyboard on the 8042 controller. Each key event
arrives as an **IRQ1** interrupt; we read the **scancode** from port `0x60`.
This is our first real device driver: hardware event → interrupt → decode →
usable input.

## Scancodes → characters

The keyboard sends **scancode set 1** by default:
- A key *press* sends a make code (e.g. `0x1E` for `A`).
- A key *release* sends the same code OR'd with `0x80`.

We keep a lookup table from make code → ASCII, track modifier state (shift for
upper case / symbols), and ignore releases except for modifiers. Special keys
(Enter, Backspace) map to `\n` and a backspace action.

## The input ring buffer

Interrupt handlers must be short and must not block, so the keyboard ISR does
the minimum: decode the scancode and, if it yields a character, push it into a
**ring buffer** (circular buffer with head/tail indices). The shell, running in
the main loop, pops characters from the buffer at its own pace.

```
IRQ1 ─► keyboard ISR ─► decode ─► ring_buffer_push(c)
                                        │
        shell loop ── ring_buffer_pop ──┘ ─► echo + line editing
```

This producer/consumer split is the standard pattern for interrupt-driven
input and keeps the ISR fast. Nutshell uses a 128-byte buffer; if it fills, the
producer drops the newest character rather than overwriting unread input.

`keyboard_getchar()` briefly disables interrupts while checking and popping
the buffer. When it is empty, the adjacent `sti; hlt` instructions enable
interrupts and sleep without a lost-wakeup race; the next timer or keyboard
IRQ resumes the loop.

## Line editing

The shell needs at least:
- printable characters → append to the current line + echo.
- **Backspace** → remove last char + erase on screen.
- **Enter** → terminate the line, hand it to the parser.

Arrow-key history is a stretch goal (the scancodes are extended, prefixed with
`0xE0`).

## Driver conventions in Nutshell

- One `.c` per device (`keyboard.c`, `timer.c`, `serial.c`, `vga.c`).
- Port I/O helpers (`inb`/`outb`) live in `include/io.h` once shared.
- Each driver exposes a small `*_init()` plus its operations; ISRs live with
  the driver and are registered through the IDT layer.

## What to demonstrate

- Typing shows characters immediately; backspace works; Enter submits.
- (In the shell milestone) the typed line is dispatched to a command.

## References

- OSDev Wiki — [PS/2 Keyboard](https://wiki.osdev.org/PS/2_Keyboard)
- OSDev Wiki — ["8042" PS/2 Controller](https://wiki.osdev.org/%228042%22_PS/2_Controller)
