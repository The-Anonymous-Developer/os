[BITS 64]
section .text

extern interrupt_handler
extern timer_handler

; Macro to create interrupt stub
%macro INTERRUPT_STUB 1
global interrupt_stub_%1
interrupt_stub_%1:
    push qword 0      ; Push dummy error code
    push qword %1     ; Push interrupt number
    jmp interrupt_common
%endmacro

; Common interrupt handling code
interrupt_common:
    ; Save all registers
    push rax
    push rcx
    push rdx
    push rbx
    push rsp
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

    ; Call C handler
    mov rdi, rsp    ; Pass pointer to stack frame
    call interrupt_handler

    ; Restore all registers
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
    pop rsp
    pop rbx
    pop rdx
    pop rcx
    pop rax

    ; Remove error code and interrupt number
    add rsp, 16
    iretq

; Create stubs for first 32 interrupts
%assign i 0
%rep 32
    INTERRUPT_STUB i
%assign i i+1
%endrep

section .data
global interrupt_stubs
interrupt_stubs:
    %assign i 0
    %rep 32
        dq interrupt_stub_%[i]
    %assign i i+1
    %endrep