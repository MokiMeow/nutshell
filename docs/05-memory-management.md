# 05 — Memory management

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

## Layer 1 — knowing what RAM exists

GRUB gives us a **memory map** in the multiboot2 info structure (the pointer
GRUB left in `ebx` at boot, which we save in milestone 0/1). It lists regions
and whether each is usable. We walk it to find the largest usable region above
the kernel image.

Key addresses we must not stomp:
- below 1 MiB: BIOS/VGA/legacy — off limits.
- the kernel image itself (`1 MiB … end`) — the linker can export an `end`
  symbol so we know where free memory starts.

## Layer 2 — physical frame allocator (`src/pmm.c`)

Hand out and reclaim 4 KiB physical frames. The simplest solid design is a
**bitmap**: one bit per frame, 0 = free, 1 = used. Mark the kernel and reserved
regions used at init; `pmm_alloc()` scans for a free bit, `pmm_free()` clears
one.

- Pros: simple, O(n) scan is fine at this scale, easy to reason about.
- Alternative: a free-list stack of frames (O(1) alloc/free) — a reasonable
  refinement, but the bitmap is clearer first.

## Layer 3 — kernel heap (`src/heap.c`)

`kmalloc(size)` / `kfree(ptr)` for byte-sized allocations. Two-stage approach:

1. **Bump allocator first** — a pointer that only moves forward. Trivial, no
   `free`. Good enough to get the shell running.
2. **Free-list heap** — blocks with headers (size + free flag), first-fit
   allocation, coalescing adjacent free blocks on `kfree`. This is the "real"
   version.

Ship the bump allocator to unblock the shell, then upgrade to the free-list
heap — each is its own commit.

## What to demonstrate in the README

- A `mem` shell command that prints total/used/free frames — proof the
  allocator tracks real state.
- A short note on bitmap vs free-list, and bump vs coalescing heap, with the
  trade-off you chose and why.

## References

- OSDev Wiki — [Page Frame Allocation](https://wiki.osdev.org/Page_Frame_Allocation)
- OSDev Wiki — [Memory Map (x86)](https://wiki.osdev.org/Memory_Map_(x86))
