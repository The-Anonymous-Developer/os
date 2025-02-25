#include "keyboard.h"
#include "hardware.h"

// Keyboard state
static int shift_pressed = 0;

// Scan code to ASCII mapping
static const char scancode_ascii[] = {
    0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    '*', 0, ' '
};

// Shifted scan code to ASCII mapping
static const char scancode_ascii_shifted[] = {
    0, 0, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',
    0, '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0,
    '*', 0, ' '
};

// Keyboard buffer
static char keyboard_buffer[KEYBOARD_BUFFER_SIZE];
static volatile uint16_t buffer_start = 0;
static volatile uint16_t buffer_end = 0;

// Initialize keyboard
void keyboard_init(void) {
    buffer_start = 0;
    buffer_end = 0;
    shift_pressed = 0;
}

// Check if there are characters in the buffer
bool keyboard_available(void) {
    return buffer_start != buffer_end;
}

// Get a character from the buffer
char keyboard_getchar(void) {
    if (!keyboard_available()) {
        return 0;
    }

    char c = keyboard_buffer[buffer_start];
    buffer_start = (buffer_start + 1) % KEYBOARD_BUFFER_SIZE;
    return c;
}

// Add a character to the buffer
static void keyboard_putchar(char c) {
    uint16_t next = (buffer_end + 1) % KEYBOARD_BUFFER_SIZE;
    
    // Only add if buffer isn't full
    if (next != buffer_start && c != 0) {
        keyboard_buffer[buffer_end] = c;
        buffer_end = next;
    }
}

void keyboard_handler(uint8_t scancode) {
    // Handle key release (bit 7 set)
    if (scancode & 0x80) {
        scancode &= 0x7F;  // Clear bit 7
        if (scancode == KEY_LSHIFT || scancode == KEY_RSHIFT) {
            shift_pressed = 0;
        }
        return;
    }

    // Handle key press
    if (scancode == KEY_LSHIFT || scancode == KEY_RSHIFT) {
        shift_pressed = 1;
        return;
    }

    // Convert scancode to ASCII
    char ascii = shift_pressed ? 
                scancode_ascii_shifted[scancode] : 
                scancode_ascii[scancode];

    // Add to keyboard buffer
    keyboard_putchar(ascii);
}