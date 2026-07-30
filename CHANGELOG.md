# Changelog

All notable changes to Nutshell are documented here. The format follows
[Keep a Changelog](https://keepachangelog.com/en/1.1.0/), and the project aims
to follow [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Changed
- Expanded the README with explicit host requirements and current kernel
  limitations, and standardized first-party Markdown punctuation.

### Fixed
- Self-test harness: raised the QEMU timeout from 25s to 60s (and the CI
  smoke-boot from 20s to 60s) so slow cold/emulated runners without KVM are not
  mistaken for failures, and reported an infrastructure timeout (exit 2)
  separately from a guest test failure (exit 1).

## [1.0.0]: 2026-07-23

### Added
- Milestone 7: a separate isa-debug-exit kernel test image, full serial-marker
  CI assertions, scripted shell coverage, and real QEMU GIF/screenshot proof.
- Milestone 6: an interactive `nutshell> ` REPL with line editing, tokenized
  command dispatch, and `help`, `echo`, `clear`, `mem`, `uptime`, `reboot`, and
  `panic` built-ins.
- Milestone 5: multiboot memory-map parsing, a bitmap physical-frame allocator,
  and a PMM-backed 64 KiB first-fit heap with block splitting and coalescing.
- Milestone 4: an IRQ1 PS/2 keyboard driver with scancode-set-1 decoding,
  Shift handling, and a fixed-size input ring buffer.
- Milestone 3: a 256-entry IDT, uniform exception/IRQ assembly stubs, fault
  reports, remapped 8259 PIC handling, and a 100 Hz PIT tick counter.
- Milestone 2: a runtime 64-bit GDT, kernel code/data descriptors, and a loaded
  TSS with dedicated ring-0 and double-fault stacks.
- Milestone 1: freestanding memory and string helpers, integer formatting, and
  `kprintf` output mirrored to VGA text mode and COM1 serial.
- Milestone 0: bootable skeleton: multiboot2 header, 32→64-bit long-mode
  bring-up (paging, PAE, EFER.LME, 64-bit GDT), minimal VGA text driver and
  COM1 serial driver, and a `kernel_main` banner.
- Build system: `Makefile` (`make`/`iso`/`run`/`debug`/`clean`), `linker.ld`,
  GRUB config, and a WSL2 toolchain setup script.
- GitHub Actions CI that builds the ISO and smoke-boots it in QEMU, verifying
  the boot banner over serial.
- Documentation set under `docs/` and the `AGENTS.md` operating manual.

## [0.1.0]: milestone 0
- First bootable version: reaches 64-bit long mode and prints a banner.
