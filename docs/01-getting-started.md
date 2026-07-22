# 01 — Getting started

Everything here is free and local. On Windows, do all of this **inside WSL2**
(Ubuntu). On Linux, run it directly.

## 1. Install the toolchain (one time)

```bash
./scripts/setup-wsl.sh
```

That installs: `nasm` (assembler), `gcc` + `binutils` (freestanding C compiler
and linker), `grub-pc-bin` + `grub-common` + `xorriso` + `mtools` (to build a
bootable ISO), and `qemu-system-x86` (the emulator we boot on).

If you prefer to do it by hand:

```bash
sudo apt-get update
sudo apt-get install -y nasm gcc make binutils \
    xorriso grub-pc-bin grub-common mtools qemu-system-x86
```

## 2. Build and boot

```bash
make run
```

This assembles `boot/*.asm`, compiles `src/*.c`, links them with `linker.ld`
into `build/kernel.bin`, wraps that in a GRUB ISO (`build/nutshell.iso`), and
boots it in QEMU. A window opens and you should see:

```
Nutshell v0.1.0 - booted into 64-bit long mode.
Milestone 0 complete. Next: docs/milestones/milestone-1-io.md
```

The same banner is printed to the serial port, which `make run` wires to your
terminal, so you'll see it there too.

## 3. Other make targets

| Command | Purpose |
|---------|---------|
| `make` | Just build `build/kernel.bin`. |
| `make iso` | Build the bootable ISO. |
| `make run` | Build + boot in QEMU. |
| `make debug` | Boot halted, GDB stub on `:1234`. |
| `make clean` | Delete `build/`. |

## 4. Capturing the boot GIF (for the README)

The boot-to-shell GIF is the project's headline artifact. Record the QEMU
window:

- **Linux**: [`peek`](https://github.com/phw/peek) or
  `ffmpeg -f x11grab ...` over the QEMU window.
- **Windows host**: use the Xbox Game Bar or ScreenToGif on the QEMU window
  running from WSL2 (WSLg forwards it to the Windows desktop).

Keep it short (5–10s): power on → banner → (later) type a command → response.

## 5. Using a cross-compiler (optional, "proper" setup)

The system `gcc` on a 64-bit host works fine for this project. If you want the
portable, canonical setup, build an `x86_64-elf` cross toolchain (see the
[OSDev GCC Cross-Compiler guide](https://wiki.osdev.org/GCC_Cross-Compiler))
and point the Makefile at it:

```bash
make run CC=x86_64-elf-gcc LD=x86_64-elf-ld
```

## Troubleshooting

- **`grub-mkrescue: command not found`** — install `grub-pc-bin grub-common`.
- **`xorriso : FAILURE`** — install `xorriso` and `mtools`.
- **Blank QEMU window** — check the serial output in your terminal; if the
  banner is there, VGA text mode just needs the window focused/redrawn.
- **`make run` hangs with no window (headless server / WSL without WSLg)** —
  boot headless and read serial instead:
  `qemu-system-x86_64 -cdrom build/nutshell.iso -display none -serial stdio`.
