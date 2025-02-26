#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>

// VGA text mode constants
#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_MEMORY 0xB8000

// Color attributes
#define VGA_BLACK     0x0
#define VGA_BLUE      0x1
#define VGA_GREEN     0x2
#define VGA_CYAN      0x3
#define VGA_RED       0x4
#define VGA_MAGENTA   0x5
#define VGA_BROWN     0x6
#define VGA_LGRAY     0x7
#define VGA_DGRAY     0x8
#define VGA_LBLUE     0x9
#define VGA_LGREEN    0xA
#define VGA_LCYAN     0xB
#define VGA_LRED      0xC
#define VGA_LMAGENTA  0xD
#define VGA_YELLOW    0xE
#define VGA_WHITE     0xF

// Function declarations
void display_init(void);
void display_clear(void);
void display_putchar(char c);
void display_write(const char* str);
void display_write_number(int num);
void display_set_color(uint8_t fg, uint8_t bg);

#endif // DISPLAY_H