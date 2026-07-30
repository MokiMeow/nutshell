# 03: Boot process

This is the most subtle part of the kernel: getting from firmware to 64-bit C
code. It is already implemented (milestone 0). This doc explains what each step
does and why.

## The chain

```
BIOS ─► GRUB ─(multiboot2)─► boot.asm (32-bit) ─► long_mode.asm (64-bit) ─► kernel_main (C)
```

### 1. GRUB and the multiboot2 header

GRUB is our bootloader. It handles the messy firmware handshake and can load
any kernel that advertises a **multiboot2 header** in its first 32 KiB. Ours is
in [`boot/multiboot_header.asm`](../boot/multiboot_header.asm): a magic number,
architecture field, length, a checksum that makes the four fields sum to zero,
and an end tag. `linker.ld` places this section first so GRUB can find it.

GRUB loads us and jumps to `start` **in 32-bit protected mode**, with `eax`
holding a magic value (`0x36d76289`) and `ebx` pointing at the multiboot info
structure (which we'll use in milestone 5 for the memory map).

### 2. Sanity checks (`boot/boot.asm`)

Before touching long mode we verify the environment:

- **Multiboot check**: `eax` must equal the magic value.
- **CPUID check**: we can use CPUID only if the ID bit (21) of EFLAGS is
  toggleable.
- **Long-mode check**: extended CPUID leaf `0x80000001` must report the LM bit
  (EDX bit 29).

If any check fails we print `ERR: <n>` to the VGA buffer and halt.

### 3. Paging: the price of admission to long mode

x86-64 **requires paging to be enabled** before entering long mode. We build a
minimal 4-level table:

```
PML4 (p4) ─► PDPT (p3) ─► PD (p2) ─► 512 × 2 MiB huge pages = 1 GiB identity map
```

Each `PD` entry maps a 2 MiB page (the `huge` bit set) at `index * 2 MiB`, so
virtual address == physical address for the first gigabyte. That's plenty for
the kernel and keeps early addressing trivial.

### 4. Enabling long mode (`enable_paging`)

Order matters:

1. Load the PML4 physical address into **CR3**.
2. Set **CR4.PAE** (bit 5): 64-bit paging needs Physical Address Extension.
3. Set **EFER.LME** (bit 8) via the `0xC0000080` MSR: "long mode enable."
4. Set **CR0.PG** (bit 31): turn paging on. The CPU is now in *compatibility*
   mode (long mode active, but still running 32-bit code).

### 5. The far jump to 64-bit

We load a 64-bit **GDT** (a null descriptor plus one 64-bit code segment) with
`lgdt`, then `jmp gdt64.code:long_mode_start`. Loading CS via a far jump is what
actually switches the CPU into 64-bit mode. From here on we execute 64-bit
instructions.

### 6. Into C (`boot/long_mode.asm`)

The 64-bit stub zeroes the data segment registers (segmentation is effectively
off in long mode, but the registers should hold the null selector) and `call`s
`kernel_main`. If `kernel_main` ever returns, we halt in a loop.

The two-entry GDT used for this transition is deliberately temporary. The
first operation in `kernel_main` is `gdt_init()`, which installs the runtime
GDT with kernel code and data segments plus a 64-bit TSS descriptor. The TSS
provides a known ring-0 stack and an IST stack reserved for double faults; the
runtime table is activated with `lgdt`, a far return reloads CS, and `ltr`
loads the task register.

## Why this design

- **GRUB + multiboot2** instead of a hand-rolled boot sector: writing a
  512-byte real-mode bootloader is a different (and more limited) project; it
  wouldn't teach us more about the *kernel*. See
  [ADR 0001](decisions/0001-multiboot2-long-mode.md).
- **Identity map, no higher half (yet)**: keeps virtual addressing out of the
  way while we build interrupts, drivers, and the shell. A higher-half
  remapping is a possible later refinement.

## References

- OSDev Wiki: [Setting Up Long Mode](https://wiki.osdev.org/Setting_Up_Long_Mode)
- OSDev Wiki: [Multiboot2](https://wiki.osdev.org/Multiboot)
- Philipp Oppermann: [Entering Long Mode](https://os.phil-opp.com/entering-longmode/)
  (Rust series, but the assembly bring-up maps directly to ours)
