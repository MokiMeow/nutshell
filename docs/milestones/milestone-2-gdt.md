# Milestone 2: Global Descriptor Table ✅ (done)

**Goal:** replace the throwaway boot GDT with a proper 64-bit GDT set up in C,
including a TSS, so the IDT (milestone 3) has a stable code selector to target.

## Concepts

Segmentation in long mode (mostly vestigial but still required), segment
descriptors and their flag/access bytes, the `lgdt` instruction, the Task State
Segment and why long mode still needs one (interrupt stack pointers).

## Tasks

- [x] Add `src/gdt.c` + `include/gdt.h` that builds a GDT with: null, 64-bit
      kernel code, kernel data, and a TSS descriptor.
- [x] Fill and load a `struct tss` (at least `rsp0` and an IST entry for a
      double-fault stack: the double-fault handler in M3 will use it).
- [x] Load the GDT with `lgdt` and reload segment registers; `ltr` to load the
      task register.
- [x] Call `gdt_init()` first thing in `kernel_main`; print `[ok] gdt`.

## Files

`src/gdt.c`, `include/gdt.h`, edit `src/kernel.c`.

## Definition of Done

- [x] Kernel boots with the C-defined GDT loaded (still reaches the banner).
- [x] The TSS is loaded (`ltr` succeeds); a known-good `rsp0`/IST is set.
- [x] `make iso` clean, `make run` boots, CI green.

## References

- OSDev Wiki: [Global Descriptor Table](https://wiki.osdev.org/Global_Descriptor_Table)
- OSDev Wiki: [GDT Tutorial](https://wiki.osdev.org/GDT_Tutorial)
- OSDev Wiki: [Task State Segment](https://wiki.osdev.org/Task_State_Segment)

**Next:** [Milestone 3: Interrupts](milestone-3-interrupts.md).
