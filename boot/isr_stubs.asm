; Interrupt and IRQ entry stubs for vectors 0-47.
;
; Entry: the CPU has pushed RIP, CS, and RFLAGS, plus an error code for the
; exceptions that define one. Interrupt gates have cleared IF.
; Exit: all general registers are restored, the uniform vector/error pair is
; removed, and iretq restores the interrupted context.

bits 64

extern isr_handler
extern irq_handler
global isr_stub_table

section .text

%macro ISR_NO_ERROR 1
global isr_stub_%1
isr_stub_%1:
    push qword 0
    push qword %1
    jmp interrupt_common
%endmacro

%macro ISR_ERROR 1
global isr_stub_%1
isr_stub_%1:
    push qword %1
    jmp interrupt_common
%endmacro

ISR_NO_ERROR 0
ISR_NO_ERROR 1
ISR_NO_ERROR 2
ISR_NO_ERROR 3
ISR_NO_ERROR 4
ISR_NO_ERROR 5
ISR_NO_ERROR 6
ISR_NO_ERROR 7
ISR_ERROR    8
ISR_NO_ERROR 9
ISR_ERROR    10
ISR_ERROR    11
ISR_ERROR    12
ISR_ERROR    13
ISR_ERROR    14
ISR_NO_ERROR 15
ISR_NO_ERROR 16
ISR_ERROR    17
ISR_NO_ERROR 18
ISR_NO_ERROR 19
ISR_NO_ERROR 20
ISR_ERROR    21
ISR_NO_ERROR 22
ISR_NO_ERROR 23
ISR_NO_ERROR 24
ISR_NO_ERROR 25
ISR_NO_ERROR 26
ISR_NO_ERROR 27
ISR_NO_ERROR 28
ISR_ERROR    29
ISR_ERROR    30
ISR_NO_ERROR 31

%assign vector 32
%rep 16
ISR_NO_ERROR vector
%assign vector vector + 1
%endrep

interrupt_common:
    push rax
    push rbx
    push rcx
    push rdx
    push rbp
    push rsi
    push rdi
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15

    cld
    mov rbx, rsp
    mov rdi, rbx
    and rsp, -16
    cmp qword [rdi + 15 * 8], 32
    jb .exception
    call irq_handler
    jmp .restore
.exception:
    call isr_handler
.restore:
    mov rsp, rbx

    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rdi
    pop rsi
    pop rbp
    pop rdx
    pop rcx
    pop rbx
    pop rax
    add rsp, 16
    iretq

section .rodata
align 8
isr_stub_table:
%assign vector 0
%rep 48
    dq isr_stub_%+vector
%assign vector vector + 1
%endrep
