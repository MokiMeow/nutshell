# 10 — Glossary

Short definitions of the terms used across these docs.

- **BIOS** — legacy firmware that initialises the machine and hands off to a
  bootloader.
- **Bump allocator** — an allocator that only moves a pointer forward; fast,
  but can't free individual allocations.
- **Compatibility mode** — long mode active but running 32-bit code, between
  enabling paging and the far jump to 64-bit.
- **CR0/CR3/CR4** — control registers. CR3 holds the page-table root; CR0.PG
  enables paging; CR4.PAE enables Physical Address Extension.
- **EFER** — Extended Feature Enable Register (MSR `0xC0000080`); its LME bit
  enables long mode.
- **EOI** — End Of Interrupt; the signal a handler sends the PIC so the IRQ can
  fire again.
- **Exception** — a CPU-generated interrupt (vectors 0–31) for faults like
  divide-by-zero or page fault.
- **Freestanding** — a C environment with no standard library or OS; only the
  compiler-provided headers (`stdint.h`, `stddef.h`, `stdbool.h`) are available.
- **GDT** — Global Descriptor Table; defines memory segments. In long mode it's
  mostly vestigial but still required (a code segment, and a TSS).
- **GRUB** — the bootloader we use; loads multiboot2 kernels.
- **Huge page** — a 2 MiB (or 1 GiB) page instead of 4 KiB; we identity-map RAM
  with 2 MiB pages at boot.
- **Identity map** — a mapping where virtual address == physical address.
- **IDT** — Interrupt Descriptor Table; maps interrupt vectors to handlers.
- **IRQ** — Interrupt ReQuest; a hardware interrupt line (e.g. IRQ0 timer,
  IRQ1 keyboard).
- **ISR** — Interrupt Service Routine; the handler that runs for an interrupt.
- **Long mode** — the x86-64 64-bit operating mode.
- **MSR** — Model-Specific Register; read/written with `rdmsr`/`wrmsr`.
- **Multiboot2** — a standard describing how a bootloader loads a kernel; our
  kernel advertises a multiboot2 header.
- **PAE** — Physical Address Extension; required for 64-bit paging.
- **PIC (8259)** — the legacy interrupt controller that delivers hardware IRQs;
  we remap it to vectors 32–47.
- **PIT (8253/8254)** — Programmable Interval Timer; fires IRQ0 at a set rate.
- **PML4 / PDPT / PD / PT** — the four levels of x86-64 page tables.
- **PS/2** — the legacy keyboard/mouse interface; our keyboard driver targets
  it via the 8042 controller.
- **Protected mode** — the 32-bit x86 mode GRUB hands control to us in.
- **Ring buffer** — a fixed-size circular FIFO; used for keyboard input.
- **Ring 0** — the most privileged CPU level; the whole kernel runs here.
- **Scancode** — the raw code a keyboard sends per key event; set 1 is the
  default.
- **Serial (UART/COM1)** — a simple byte stream we use for logging; QEMU
  forwards it to the host.
- **Triple fault** — three nested faults with no handler; the CPU resets (looks
  like a silent reboot).
- **TSS** — Task State Segment; in long mode it mainly holds stack pointers for
  privilege/interrupt handling.
- **VGA text mode** — an 80×25 grid of character+attribute cells at physical
  `0xB8000`; how we print to screen early.
