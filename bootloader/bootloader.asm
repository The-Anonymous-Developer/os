BITS 64
DEFAULT REL

; EFI Application Entry Point
global EfiMain

section .text
EfiMain:
    ; Function parameters:
    ; rcx: EFI_HANDLE ImageHandle
    ; rdx: EFI_SYSTEM_TABLE *SystemTable
    push rbp
    mov rbp, rsp
    
    ; Save parameters
    mov [ImageHandle], rcx
    mov [SystemTable], rdx

    ; Get ConOut pointer
    mov rcx, [SystemTable]
    mov rcx, [rcx + 64]    ; ConOut offset in system table
    mov [ConOut], rcx

    ; Clear screen
    mov rcx, [ConOut]
    mov rax, [rcx + 8]     ; ClearScreen function
    mov rcx, [ConOut]
    sub rsp, 32            ; Reserve shadow space
    call rax
    add rsp, 32

    ; Print string
    mov rcx, [ConOut]
    mov rax, [rcx + 16]    ; OutputString function
    mov rcx, [ConOut]
    lea rdx, [hello]
    sub rsp, 32
    call rax
    add rsp, 32

    ; Return EFI_SUCCESS
    xor rax, rax
    leave
    ret

section .data
    ImageHandle  dq 0
    SystemTable  dq 0
    ConOut      dq 0
    hello       dw 'H',0,'e',0,'l',0,'l',0,'o',0,' ',0
                dw 'W',0,'o',0,'r',0,'l',0,'d',0,'!',0
                dw 13,0,10,0,0,0    ; CR,LF,NUL (Unicode)