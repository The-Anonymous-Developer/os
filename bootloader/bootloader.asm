BITS 64
DEFAULT REL

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
    sub rsp, 32
    call rax
    add rsp, 32

    ; Print string: Bootloader initialized
    mov rcx, [ConOut]
    mov rax, [rcx + 16]    ; OutputString function
    mov rcx, [ConOut]
    lea rdx, [boot_msg]
    sub rsp, 32
    call rax
    add rsp, 32

    ; Load Kernel
    call LoadKernel

    ; Return EFI_SUCCESS
    xor rax, rax
    leave
    ret

; ------------------------------------------------------------
; LOAD KERNEL FUNCTION
; ------------------------------------------------------------
LoadKernel:
    ; Get Boot Services table
    mov rbx, [SystemTable]
    mov rbx, [rbx + 24]    ; BootServices offset in system table

    ; Open root directory
    mov rax, [rbx + 248]   ; LocateProtocol
    mov rcx, FileSystemGuid
    mov rdx, 0
    mov r8, FileSystemHandle
    sub rsp, 32
    call rax
    add rsp, 32

    ; Open file: kernel.elf
    mov rax, [FileSystemHandle]
    mov rax, [rax + 48]    ; OpenFile
    mov rcx, [FileSystemHandle]
    lea rdx, [kernel_path]
    mov r8, FileHandle
    mov r9, 1              ; EFI_FILE_MODE_READ
    sub rsp, 32
    call rax
    add rsp, 32

    ; Read file into memory
    mov rax, [FileHandle]
    mov rax, [rax + 56]    ; Read
    mov rcx, [FileHandle]
    mov rdx, FileSize
    mov r8, KernelBuffer
    sub rsp, 32
    call rax
    add rsp, 32

    ; Close file
    mov rax, [FileHandle]
    mov rax, [rax + 16]    ; Close
    mov rcx, [FileHandle]
    sub rsp, 32
    call rax
    add rsp, 32

    ; Jump to Kernel (assumes identity mapping)
    mov rax, KernelBuffer
    call rax
    ret

section .data
    ImageHandle  dq 0
    SystemTable  dq 0
    ConOut       dq 0
    FileSystemHandle dq 0
    FileHandle   dq 0
    KernelBuffer dq 0x100000   ; Load kernel at 1MB
    FileSize     dq 0x200000   ; Assume max size 2MB

    boot_msg     dw 'B',0,'o',0,'o',0,'t',0,'l',0,'o',0,'a',0,'d',0,'e',0,'r',0,' ',0
                 dw 'I',0,'n',0,'i',0,'t',0,'i',0,'a',0,'l',0,'i',0,'z',0,'e',0,'d',0
                 dw 13,0,10,0,0,0  ; CR, LF, NULL

    kernel_path  db '../build/bin/kernel.elf',0
    FileSystemGuid dq 0x0964e5b22, 0x6459f683, 0x64a2b4c5
