section .text
global write_cr3
global read_cr3
global CLI
global STI
global halt

write_cr3:
    mov cr3, rcx    ; First parameter is in RCX in Windows x64 calling convention
    ret

read_cr3:
    mov rax, cr3    ; Return value in RAX
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