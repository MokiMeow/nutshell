# ADR 0001: Use GRUB/Multiboot2 and hand-write the long-mode transition

**Status:** accepted · **Date:** 2026

## Context

To run our own x86-64 kernel we must get from firmware to 64-bit C. Options:

1. Write a real-mode boot sector from scratch (own 512-byte bootloader).
2. Use GRUB via the multiboot2 standard; write the kernel bring-up ourselves.
3. Use a modern bootloader (e.g. Limine) that drops us straight into 64-bit.

## Decision

Use **GRUB + multiboot2**, and **hand-write the 32→64-bit transition**
(CPUID checks, page tables, PAE, EFER.LME, paging, 64-bit GDT, far jump).

## Rationale

- A from-scratch real-mode bootloader is a *different* project (real-mode, BIOS
  disk quirks, 512-byte limits). It teaches BIOS trivia, not kernel internals,
  which are the point here.
- GRUB handles the firmware handshake reliably across BIOS setups and gives us
  a memory map for free (used in milestone 5).
- Writing the long-mode transition *ourselves*, rather than letting a modern
  bootloader do it: keeps the most instructive part (paging + mode switch) in
  our own code. That's the depth we want on show.

## Consequences

- We depend on GRUB + `grub-mkrescue` to build the ISO (extra host packages).
- We boot via BIOS/legacy, which gives us VGA text mode at `0xB8000` for free.
- A UEFI boot path and a modern-bootloader path are possible later but are not
  needed for v1.
