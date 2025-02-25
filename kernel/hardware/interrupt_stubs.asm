BITS 64
DEFAULT REL

SECTION .text

; Export interrupt stubs
global interrupt_stub_0
global interrupt_stub_1
global interrupt_stub_2
global interrupt_stub_3
global interrupt_stub_4
global interrupt_stub_5
global interrupt_stub_6
global interrupt_stub_7
global interrupt_stub_8
global interrupt_stub_9
global interrupt_stub_10
global interrupt_stub_11
global interrupt_stub_12
global interrupt_stub_13
global interrupt_stub_14
global interrupt_stub_15
global interrupt_stub_16
global interrupt_stub_17
global interrupt_stub_18
global interrupt_stub_19
global interrupt_stub_20
global interrupt_stub_21
global interrupt_stub_22
global interrupt_stub_23
global interrupt_stub_24
global interrupt_stub_25
global interrupt_stub_26
global interrupt_stub_27
global interrupt_stub_28
global interrupt_stub_29
global interrupt_stub_30
global interrupt_stub_31

; Import C handler
extern interrupt_handler

; Common handler
common_handler:
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

    mov rcx, rsp    ; First parameter for Windows x64
    sub rsp, 32     ; Shadow space
    call interrupt_handler
    add rsp, 32     ; Restore stack

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

    add rsp, 16     ; Remove error code and interrupt number
    iretq

; Interrupt stubs
interrupt_stub_0:
    push qword 0    ; Error code
    push qword 0    ; Interrupt number
    jmp common_handler

interrupt_stub_1:
    push qword 0
    push qword 1
    jmp common_handler

interrupt_stub_2:
    push qword 0
    push qword 2
    jmp common_handler

interrupt_stub_3:
    push qword 0
    push qword 3
    jmp common_handler

interrupt_stub_4:
    push qword 0
    push qword 4
    jmp common_handler

interrupt_stub_5:
    push qword 0
    push qword 5
    jmp common_handler

interrupt_stub_6:
    push qword 0
    push qword 6
    jmp common_handler

interrupt_stub_7:
    push qword 0
    push qword 7
    jmp common_handler

interrupt_stub_8:
    push qword 0
    push qword 8
    jmp common_handler

interrupt_stub_9:
    push qword 0
    push qword 9
    jmp common_handler

interrupt_stub_10:
    push qword 0
    push qword 10
    jmp common_handler

interrupt_stub_11:
    push qword 0
    push qword 11
    jmp common_handler

interrupt_stub_12:
    push qword 0
    push qword 12
    jmp common_handler

interrupt_stub_13:
    push qword 0
    push qword 13
    jmp common_handler

interrupt_stub_14:
    push qword 0
    push qword 14
    jmp common_handler

interrupt_stub_15:
    push qword 0
    push qword 15
    jmp common_handler

interrupt_stub_16:
    push qword 0
    push qword 16
    jmp common_handler

interrupt_stub_17:
    push qword 0
    push qword 17
    jmp common_handler

interrupt_stub_18:
    push qword 0
    push qword 18
    jmp common_handler

interrupt_stub_19:
    push qword 0
    push qword 19
    jmp common_handler

interrupt_stub_20:
    push qword 0
    push qword 20
    jmp common_handler

interrupt_stub_21:
    push qword 0
    push qword 21
    jmp common_handler

interrupt_stub_22:
    push qword 0
    push qword 22
    jmp common_handler

interrupt_stub_23:
    push qword 0
    push qword 23
    jmp common_handler

interrupt_stub_24:
    push qword 0
    push qword 24
    jmp common_handler

interrupt_stub_25:
    push qword 0
    push qword 25
    jmp common_handler

interrupt_stub_26:
    push qword 0
    push qword 26
    jmp common_handler

interrupt_stub_27:
    push qword 0
    push qword 27
    jmp common_handler

interrupt_stub_28:
    push qword 0
    push qword 28
    jmp common_handler

interrupt_stub_29:
    push qword 0
    push qword 29
    jmp common_handler

interrupt_stub_30:
    push qword 0
    push qword 30
    jmp common_handler

interrupt_stub_31:
    push qword 0
    push qword 31
    jmp common_handler