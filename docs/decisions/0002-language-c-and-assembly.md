# ADR 0002 — Write the kernel in C and assembly (not Rust)

**Status:** accepted · **Date:** 2026

## Context

The kernel needs a systems language with no runtime and direct hardware access.
Realistic choices: C + assembly, or Rust + assembly.

## Decision

**C (C11) + NASM assembly**, freestanding, no libc.

## Rationale

- C is the lingua franca of OS bring-up: every reference (OSDev, textbooks,
  datasheets) is in C or pseudo-C, so the mapping from documentation to code is
  direct.
- Writing memory management and pointer-level hardware access *in C* — without
  a borrow checker mediating — is exactly the "understand the bare metal"
  signal this project exists to show.
- Assembly is unavoidable for the boot transition and interrupt stubs; NASM is
  the standard.
- (Portfolio context: Rust is already the language of other projects in this
  series, so C here maximises language coverage.)

## Consequences

- No memory-safety net — discipline and testing (QEMU, GDB, serial logs) carry
  that weight. See [docs/09](../09-testing-and-debugging.md).
- We implement our own `memcpy`/`memset`/`strlen` as needed (`src/string.c`).
- Build flags disable SSE/MMX/red zone and the stack protector, because the
  kernel has no runtime to support them (see the Makefile).
