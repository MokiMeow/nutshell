# Milestone 5 — Memory (physical allocator + kernel heap)

**Goal:** dynamic memory in the kernel — a physical frame allocator over the
real memory map, and a `kmalloc`/`kfree` heap on top.

## Concepts

The multiboot2 memory map, physical vs virtual (we're identity-mapped),
bitmap frame allocation, bump vs free-list heaps, block headers and coalescing.

## Tasks

- [ ] Save the multiboot2 info pointer at boot (GRUB leaves it in `ebx`; pass it
      from `long_mode.asm` into `kernel_main` as an argument, or stash it).
- [ ] Export an `end` symbol from `linker.ld` so the kernel knows where its
      image ends and free RAM can begin.
- [ ] `src/pmm.c` + `include/pmm.h`: parse the memory map, build a **bitmap**
      frame allocator; `pmm_alloc()` / `pmm_free()`; track total/used/free.
- [ ] `src/heap.c` + `include/heap.h`: `kmalloc`/`kfree`. Ship a **bump**
      allocator first to unblock the shell, then upgrade to a **free-list** heap
      with coalescing (separate commit).
- [ ] Expose `mem_stats()` (total/used/free frames + heap usage) for the `mem`
      command.
- [ ] Print `[ok] pmm`, `[ok] heap`.

## Files

`src/pmm.c`, `include/pmm.h`, `src/heap.c`, `include/heap.h`,
edit `linker.ld` (add `end`), `boot/long_mode.asm` + `src/kernel.c` (pass the
multiboot pointer).

## Definition of Done

- [ ] `pmm_alloc()` returns distinct valid frames; `pmm_free()` reclaims them
      (a self-test at init: alloc N, free, re-alloc, assert reuse).
- [ ] `kmalloc`/`kfree` work; a free-list stress test (alloc/free mix) doesn't
      corrupt the heap.
- [ ] `mem_stats()` reports plausible numbers.
- [ ] `make iso` clean, `make run` boots, CI green.

## References

- OSDev Wiki — [Page Frame Allocation](https://wiki.osdev.org/Page_Frame_Allocation)
- OSDev Wiki — [Memory Map (x86)](https://wiki.osdev.org/Memory_Map_(x86))
- OSDev Wiki — [Writing a memory manager](https://wiki.osdev.org/Memory_Allocation)

**Next:** [Milestone 6 — The shell](milestone-6-shell.md).
