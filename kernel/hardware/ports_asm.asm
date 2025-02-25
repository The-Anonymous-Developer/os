section .text
global outb
global inb
global io_wait

; void outb(uint16_t port, uint8_t value)
outb:
    mov dx, di    ; first argument (port)
    mov al, sil   ; second argument (value)
    out dx, al
    ret

; uint8_t inb(uint16_t port)
inb:
    mov dx, di    ; first argument (port)
    in al, dx
    ret

; void io_wait(void)
io_wait:
    mov al, 0
    out 0x80, al
    ret