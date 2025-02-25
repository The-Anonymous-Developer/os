section .text
global write_cr3
global read_cr3
global read_cr2
global CLI
global STI
global halt

write_cr3:
    mov cr3, rcx    ; First parameter in Windows x64
    ret

read_cr3:
    mov rax, cr3    ; Return value in RAX
    ret

read_cr2:
    mov rax, cr2    ; Get page fault address
    ret

CLI:
    cli
    ret

STI:
    sti
    ret

halt:
    hlt
    ret