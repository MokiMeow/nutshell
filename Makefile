# Nutshell — build system
#
# Quick start (WSL2 / Linux):
#   ./scripts/setup-wsl.sh   # one-time: install the toolchain
#   make run                 # build the ISO and boot it in QEMU
#
# Override the toolchain if you use a cross-compiler, e.g.:
#   make run CC=x86_64-elf-gcc LD=x86_64-elf-ld

CC            := gcc
LD            := ld
NASM          := nasm
QEMU          := qemu-system-x86_64
GRUB_MKRESCUE := grub-mkrescue

CFLAGS  := -std=c11 -m64 -ffreestanding -fno-pic -fno-pie \
           -mno-red-zone -mno-mmx -mno-sse -mno-sse2 \
           -fno-stack-protector -fno-asynchronous-unwind-tables \
           -Wall -Wextra -Iinclude -c
LDFLAGS := -n -T linker.ld

BUILD   := build
ISO_DIR := $(BUILD)/isofiles
KERNEL  := $(BUILD)/kernel.bin
ISO     := $(BUILD)/nutshell.iso

ASM_SRC := $(wildcard boot/*.asm)
C_SRC   := $(wildcard src/*.c)
ASM_OBJ := $(patsubst boot/%.asm,$(BUILD)/%.o,$(ASM_SRC))
C_OBJ   := $(patsubst src/%.c,$(BUILD)/%.o,$(C_SRC))
OBJ     := $(ASM_OBJ) $(C_OBJ)

.PHONY: all iso run debug clean

all: $(KERNEL)

$(BUILD)/%.o: boot/%.asm | $(BUILD)
	$(NASM) -f elf64 $< -o $@

$(BUILD)/%.o: src/%.c | $(BUILD)
	$(CC) $(CFLAGS) $< -o $@

$(KERNEL): $(OBJ) linker.ld
	$(LD) $(LDFLAGS) -o $@ $(OBJ)

$(BUILD):
	mkdir -p $(BUILD)

iso: $(ISO)

$(ISO): $(KERNEL) grub/grub.cfg
	mkdir -p $(ISO_DIR)/boot/grub
	cp $(KERNEL) $(ISO_DIR)/boot/kernel.bin
	cp grub/grub.cfg $(ISO_DIR)/boot/grub/grub.cfg
	$(GRUB_MKRESCUE) -o $(ISO) $(ISO_DIR) 2>/dev/null

# Boot the kernel. Serial output is mirrored to your terminal.
run: $(ISO)
	$(QEMU) -cdrom $(ISO) -serial stdio

# Boot halted, waiting for GDB on :1234 (connect: target remote :1234)
debug: $(ISO)
	$(QEMU) -cdrom $(ISO) -serial stdio -s -S

clean:
	rm -rf $(BUILD)
