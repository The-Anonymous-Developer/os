section .text
global CLI
global STI
global halt

; void CLI(void)
CLI:
    cli
    ret

; void STI(void)
STI:
    sti
    ret

; void halt(void)
halt:
    hlt
    ret