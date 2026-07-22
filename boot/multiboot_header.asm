; Multiboot2 header.
;
; This is what GRUB scans for to recognise the file as a bootable kernel.
; It must live in the first 32 KiB of the binary and be 8-byte aligned;
; the linker script places the `.multiboot_header` section first to guarantee
; this. Spec: https://www.gnu.org/software/grub/manual/multiboot2/

section .multiboot_header
header_start:
    dd 0xe85250d6                 ; magic number (multiboot2)
    dd 0                          ; architecture 0 (i386 protected mode)
    dd header_end - header_start  ; header length
    ; checksum: magic + arch + length + checksum must equal 0 (mod 2^32)
    dd 0x100000000 - (0xe85250d6 + 0 + (header_end - header_start))

    ; required end tag
    dw 0    ; type
    dw 0    ; flags
    dd 8    ; size
header_end:
