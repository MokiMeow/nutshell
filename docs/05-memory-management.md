# 05: Memory management

*Implemented in milestone 5.* This doc is the concept reference; the build
steps and Definition of Done are in
[milestones/milestone-5-memory.md](milestones/milestone-5-memory.md).

## The problem

After boot we have paging on and the first 1 GiB identity-mapped, but no way to
*allocate* memory. Global and static data live in `.bss`; everything else would
have to be a fixed array. The shell and future subsystems want dynamic memory.
We build two layers:

```
kmalloc / kfree        ← kernel heap (byte-granular)
        │
physical frame alloc   ← hands out 4 KiB frames
        │
multiboot memory map   ← what RAM actually exists
```

## Layer 1: knowing what RAM exists

GRUB gives us a **memory map** in the multiboot2 info structure (the pointer
GRUB left in `ebx` at boot, which the 32-bit entry saves before CPUID can
clobber it and the 64-bit stub passes to `kernel_main`). It lists regions
and whether each is usable. We walk it to find the largest usable region above
the kernel image.

Key addresses we must not stomp:
- below 1 MiB: BIOS/VGA/legacy: off limits.
- the kernel image itself (`1 MiB … end`): the linker can export an `end`
symbol so we know where free memory starts.
- the multiboot info structure itself: it remains reserved after parsing.

Only frames below **1 GiB** are managed because that is the region mapped by
the early page tables. Returning higher physical addresses before extending
the page tables would hand callers inaccessible memory.

## Layer 2: physical frame allocator (`src/pmm.c`)

Hand out and reclaim 4 KiB physical frames. The simplest solid design is a
**bitmap**: one bit per frame, 0 = free, 1 = used. Mark the kernel and reserved
regions used at init; `pmm_alloc()` scans for a free bit, `pmm_free()` clears
one. A second ownership bitmap records which used frames were actually handed
out, so invalid frees cannot release the kernel, multiboot data, or reserved
hardware regions.

- Pros: simple, O(n) scan is fine at this scale, easy to reason about.
- Alternative: a free-list stack of frames (O(1) alloc/free): a reasonable
  refinement, but the bitmap is clearer first.

## Layer 3: kernel heap (`src/heap.c`)

`kmalloc(size)` / `kfree(ptr)` for byte-sized allocations. Two-stage approach:

1. **Bump allocator first**: a pointer that only moves forward. Trivial, no
   `free`. Good enough to get the shell running.
2. **Free-list heap**: blocks with headers (size + free flag), first-fit
   allocation, coalescing adjacent free blocks on `kfree`. This is the "real"
   version.

Ship the bump allocator to unblock the shell, then upgrade to the free-list
heap: each is its own commit.

The final heap owns 16 consecutive PMM frames (64 KiB). Payloads are aligned to
16 bytes. Allocation uses first-fit and splits blocks when the remainder is
useful; freeing merges adjacent blocks in both directions. The boot self-test
mixes allocations and frees, verifies live payload contents, and checks that
the arena coalesces back into one block.

## What to demonstrate in the README

- A `mem` shell command that prints total/used/free frames: proof the
  allocator tracks real state.
- A short note on bitmap vs free-list, and bump vs coalescing heap, with the
  trade-off you chose and why.

## References

- OSDev Wiki: [Page Frame Allocation](https://wiki.osdev.org/Page_Frame_Allocation)
- OSDev Wiki: [Memory Map (x86)](https://wiki.osdev.org/Memory_Map_(x86))
