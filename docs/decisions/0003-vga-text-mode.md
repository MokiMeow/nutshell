# ADR 0003: Use VGA text mode (not a framebuffer) for early output

**Status:** accepted · **Date:** 2026

## Context

The kernel needs to put characters on screen. Two paths on our BIOS boot:

1. **VGA text mode**: write 16-bit cells to `0xB8000`; hardware renders glyphs.
2. **Linear framebuffer**: request one from GRUB and draw pixels/fonts
   ourselves.

## Decision

Use **VGA text mode** for v1, plus a **COM1 serial** channel for logging.

## Rationale

- VGA text mode needs zero initialisation on the legacy BIOS path: writing to
  `0xB8000` immediately shows text. That gets us debuggable output on line one
  of C, which matters most during bring-up.
- A framebuffer means shipping a font and a glyph renderer before we can print
  "hello": effort spent on presentation, not on kernel internals.
- Serial complements the screen: it's trivial to capture, scroll, and grep, and
  it's what CI checks.

## Consequences

- Output is 80×25, monochrome-ish (16 colours), no graphics. Fine for a shell.
- A framebuffer + font renderer is a clean stretch goal if graphical output is
  ever wanted, and would not disturb the rest of the kernel.
