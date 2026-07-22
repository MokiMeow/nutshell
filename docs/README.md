# Nutshell documentation

Start here. These docs explain both *what* Nutshell is and *how* to build it,
piece by piece, to a working shell.

## Read in this order

1. [00 — Overview](00-overview.md) — the big picture and design goals.
2. [01 — Getting started](01-getting-started.md) — toolchain, build, run, GIF.
3. [02 — Architecture](02-architecture.md) — how the pieces fit together.
4. [03 — Boot process](03-boot-process.md) — GRUB → 32-bit → 64-bit long mode.
5. [04 — Roadmap](04-roadmap.md) — the milestone plan from boot to shell.

## Concept references (per subsystem)

- [05 — Memory management](05-memory-management.md)
- [06 — Interrupts](06-interrupts.md)
- [07 — Keyboard & drivers](07-keyboard-and-drivers.md)
- [08 — The shell](08-the-shell.md)
- [09 — Testing & debugging](09-testing-and-debugging.md)
- [10 — Glossary](10-glossary.md)

## Milestones

Step-by-step specs, each with a Definition of Done, live in
[milestones/](milestones/).

## Design decisions

The *why* behind the big choices lives in [decisions/](decisions/) (ADRs).
