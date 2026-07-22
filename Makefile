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
# --no-warn-rwx-segments: the kernel is one flat LOAD segment (R+W+X) on
# purpose; silence the otherwise-harmless linker warning about it.
LDFLAGS := -n -T linker.ld --no-warn-rwx-segments

BUILD   := build
ISO_DIR := $(BUILD)/isofiles
KERNEL  := $(BUILD)/kernel.bin
ISO     := $(BUILD)/nutshell.iso
TEST_BUILD   := $(BUILD)/test
TEST_ISO_DIR := $(TEST_BUILD)/isofiles
TEST_KERNEL  := $(TEST_BUILD)/kernel.bin
TEST_ISO     := $(BUILD)/nutshell-test.iso

ASM_SRC := $(wildcard boot/*.asm)
C_SRC   := $(wildcard src/*.c)
ASM_OBJ := $(patsubst boot/%.asm,$(BUILD)/%.o,$(ASM_SRC))
C_OBJ   := $(patsubst src/%.c,$(BUILD)/%.o,$(C_SRC))
OBJ     := $(ASM_OBJ) $(C_OBJ)
TEST_ASM_OBJ := $(patsubst boot/%.asm,$(TEST_BUILD)/%.o,$(ASM_SRC))
TEST_C_OBJ   := $(patsubst src/%.c,$(TEST_BUILD)/%.o,$(C_SRC))
TEST_OBJ     := $(TEST_ASM_OBJ) $(TEST_C_OBJ)

.PHONY: all iso test-iso test run debug clean

all: $(KERNEL)

$(BUILD)/%.o: boot/%.asm | $(BUILD)
	$(NASM) -f elf64 $< -o $@

$(BUILD)/%.o: src/%.c | $(BUILD)
	$(CC) $(CFLAGS) $< -o $@

$(KERNEL): $(OBJ) linker.ld
	$(LD) $(LDFLAGS) -o $@ $(OBJ)

$(BUILD):
	mkdir -p $(BUILD)

$(TEST_BUILD)/%.o: boot/%.asm | $(TEST_BUILD)
	$(NASM) -f elf64 $< -o $@

$(TEST_BUILD)/%.o: src/%.c | $(TEST_BUILD)
	$(CC) $(CFLAGS) -DKERNEL_TEST $< -o $@

$(TEST_KERNEL): $(TEST_OBJ) linker.ld
	$(LD) $(LDFLAGS) -o $@ $(TEST_OBJ)

$(TEST_BUILD):
	mkdir -p $(TEST_BUILD)

iso: $(ISO)

$(ISO): $(KERNEL) grub/grub.cfg
	mkdir -p $(ISO_DIR)/boot/grub
	cp $(KERNEL) $(ISO_DIR)/boot/kernel.bin
	cp grub/grub.cfg $(ISO_DIR)/boot/grub/grub.cfg
	$(GRUB_MKRESCUE) -o $(ISO) $(ISO_DIR) 2>/dev/null

test-iso: $(TEST_ISO)

$(TEST_ISO): $(TEST_KERNEL) grub/grub.cfg
	mkdir -p $(TEST_ISO_DIR)/boot/grub
	cp $(TEST_KERNEL) $(TEST_ISO_DIR)/boot/kernel.bin
	cp grub/grub.cfg $(TEST_ISO_DIR)/boot/grub/grub.cfg
	$(GRUB_MKRESCUE) -o $(TEST_ISO) $(TEST_ISO_DIR) 2>/dev/null

test: $(TEST_ISO)
	bash tests/run-selftest.sh $(TEST_ISO)

# Boot the kernel. Serial output is mirrored to your terminal.
run: $(ISO)
	$(QEMU) -cdrom $(ISO) -serial stdio

# Boot halted, waiting for GDB on :1234 (connect: target remote :1234)
debug: $(ISO)
	$(QEMU) -cdrom $(ISO) -serial stdio -s -S

clean:
	rm -rf $(BUILD)
