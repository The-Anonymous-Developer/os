#include "display.h"

static uint16_t* vga_buffer = (uint16_t*)VGA_MEMORY;
static uint8_t vga_color = (VGA_LGRAY | (VGA_BLACK << 4));
static uint16_t cursor_x = 0;
static uint16_t cursor_y = 0;

void display_init(void) {
    display_clear();
}

void display_clear(void) {
    for(int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        vga_buffer[i] = (vga_color << 8) | ' ';
    }
    cursor_x = cursor_y = 0;
}

static void scroll(void) {
    if(cursor_y >= VGA_HEIGHT) {
        // Move everything up one line
        for(int i = 0; i < (VGA_HEIGHT - 1) * VGA_WIDTH; i++) {
            vga_buffer[i] = vga_buffer[i + VGA_WIDTH];
        }

        // Clear bottom line
        for(int i = (VGA_HEIGHT - 1) * VGA_WIDTH; i < VGA_HEIGHT * VGA_WIDTH; i++) {
            vga_buffer[i] = (vga_color << 8) | ' ';
        }

        cursor_y = VGA_HEIGHT - 1;
    }
}

void display_putchar(char c) {
    switch(c) {
        case '\n':
            cursor_x = 0;
            cursor_y++;
            break;
            
        case '\r':
            cursor_x = 0;
            break;
            
        case '\b':
            if(cursor_x > 0) {
                cursor_x--;
                vga_buffer[cursor_y * VGA_WIDTH + cursor_x] = (vga_color << 8) | ' ';
            }
            break;
            
        default:
            vga_buffer[cursor_y * VGA_WIDTH + cursor_x] = (vga_color << 8) | c;
            cursor_x++;
            if(cursor_x >= VGA_WIDTH) {
                cursor_x = 0;
                cursor_y++;
            }
    }
    
    scroll();
}

void display_write(const char* str) {
    while(*str) {
        display_putchar(*str++);
    }
}

void display_set_color(uint8_t fg, uint8_t bg) {
    vga_color = fg | (bg << 4);
}