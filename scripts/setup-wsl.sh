#!/usr/bin/env bash
# One-time toolchain setup for Ubuntu / WSL2.
# After this, `make run` builds the ISO and boots Nutshell in QEMU.
set -euo pipefail

echo ">> Installing the Nutshell toolchain (nasm, gcc, binutils, grub, qemu)..."
sudo apt-get update
sudo apt-get install -y \
    nasm \
    gcc \
    make \
    binutils \
    xorriso \
    grub-pc-bin \
    grub-common \
    mtools \
    qemu-system-x86

echo
echo ">> Done. Build and boot with:"
echo "     make run"
