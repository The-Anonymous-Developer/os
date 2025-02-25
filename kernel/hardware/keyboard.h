#ifndef KEYBOARD_H
#define KEYBOARD_H
#include <stdbool.h>

#include <stdint.h>

#define KEYBOARD_BUFFER_SIZE 256

// Keyboard scan codes
#define KEY_LSHIFT   0x2A
#define KEY_RSHIFT   0x36
#define KEY_CAPS     0x3A
#define KEY_ENTER    0x1C
#define KEY_BACKSPACE 0x0E

// Function declarations
void keyboard_init(void);
void keyboard_handler(uint8_t scancode);
char keyboard_getchar(void);
bool keyboard_has_char(void);
bool keyboard_available(void);  // Added this declaration

#endif // KEYBOARD_H