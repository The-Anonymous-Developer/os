#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>
#include <stdbool.h>

#define KEYBOARD_BUFFER_SIZE 256

// Key definitions
#define KEY_ESC          0x01
#define KEY_ENTER        0x1C
#define KEY_SPACE        0x39
#define KEY_BACKSPACE    0x0E
#define KEY_LSHIFT       0x2A
#define KEY_RSHIFT       0x36

// Function declarations
void keyboard_handler(uint8_t scancode);
char keyboard_getchar(void);
bool keyboard_available(void);
void keyboard_init(void);

#endif // KEYBOARD_H