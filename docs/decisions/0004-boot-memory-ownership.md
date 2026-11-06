# 0004: Conservative boot-memory ownership

Date: 2026-11-06

## Context and research

The original PMM stopped parsing at the first memory-map tag and treated every
available entry as free RAM. That could overwrite modules loaded by GRUB, and
could release firmware-reserved holes when memory-map entries overlapped. The
three-allocation boot test did not exercise malformed or overlapping maps.

The current GNU [Multiboot2 specification, sections 3.6.2, 3.6.6 and 3.6.8](https://www.gnu.org/software/grub/manual/multiboot2/html_node/Boot-information-format.html)
states that available RAM may include the kernel, boot information and modules.
Tags are eight-byte aligned and terminate with a type-zero, size-eight tag.
Memory-map strides are extensible multiples of eight; future versions preserve
the common prefix. Reserved fields must be ignored, not required to be zero.
Module commands are NUL-terminated strings within the tag size.

The OSDev [memory detection guide](https://wiki.osdev.org/Detecting_Memory_(x86))
recommends conservative handling of overlapping firmware ranges, unknown types,
and non-page-aligned boundaries. These sources were checked on 2026-11-06.

## Decision

Keep the existing small bitmap allocator and one-GiB identity-map boundary.
Validate the entire Multiboot2 chain before configuring frames. Reject malformed
records and ambiguous duplicate maps, while skipping unknown tags and accepting
compatible extended entries. Ignore reserved fields as specified.

Free only wholly available pages, then reserve all pages touched by unavailable
entries. Reserve the kernel, boot information and every module afterward. Clip
ranges before arithmetic and treat empty ranges as empty. On initialization
failure, leave the allocator disabled.

Exercise the same production code with synthetic maps in both a native test
executable and the QEMU test kernel. The native build additionally mutates every
byte in a valid tag list through all 256 values under sanitizers. Run synthetic
initializations before heap setup and restore the real GRUB map immediately
afterward.

## Consequences

Boot data is checked once, with a few bounded linear passes. Allocation policy
and ownership tracking remain easy to inspect; no general parser framework or
external dependency is added to the kernel. Host tests need only GCC and make,
but do not replace boot tests. Invalid boot data now produces a clear PMM init
failure rather than a partially usable allocator. Broader paging protection,
SMP synchronization and a more advanced allocation strategy remain separate work.
