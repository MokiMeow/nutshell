; 64-bit entry point.
;
; We reach here only after boot.asm has enabled long mode and far-jumped
; using the 64-bit code selector. Reload the data segment registers with the
; null selector (they are ignored in long mode but should be clean) and hand
; control to the C kernel.

global long_mode_start
extern kernel_main
extern multiboot_info

section .text
bits 64
long_mode_start:
    mov ax, 0
    mov ss, ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov edi, [rel multiboot_info] ; SysV arg 1: multiboot2 pointer
    call kernel_main

    ; kernel_main should never return; if it does, halt forever.
.hang:
    hlt
    jmp .hang
