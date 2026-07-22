# Changelog

All notable changes to Nutshell are documented here. The format follows
[Keep a Changelog](https://keepachangelog.com/en/1.1.0/), and the project aims
to follow [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added
- Milestone 2: a runtime 64-bit GDT, kernel code/data descriptors, and a loaded
  TSS with dedicated ring-0 and double-fault stacks.
- Milestone 1: freestanding memory and string helpers, integer formatting, and
  `kprintf` output mirrored to VGA text mode and COM1 serial.
- Milestone 0: bootable skeleton — multiboot2 header, 32→64-bit long-mode
  bring-up (paging, PAE, EFER.LME, 64-bit GDT), minimal VGA text driver and
  COM1 serial driver, and a `kernel_main` banner.
- Build system: `Makefile` (`make`/`iso`/`run`/`debug`/`clean`), `linker.ld`,
  GRUB config, and a WSL2 toolchain setup script.
- GitHub Actions CI that builds the ISO and smoke-boots it in QEMU, verifying
  the boot banner over serial.
- Documentation set under `docs/` and the `AGENTS.md` operating manual.

## [0.1.0] — milestone 0
- First bootable version: reaches 64-bit long mode and prints a banner.
