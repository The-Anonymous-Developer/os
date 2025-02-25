#include "keyboard.h"
#include "ports.h"
#include "../hardware/interrupt.h"
#include "../lib/string.h"
#include "../hardware/cpu.h"

// Keyboard buffer
static char keyboard_buffer[KEYBOARD_BUFFER_SIZE];
static uint16_t buffer_start = 0;
static uint16_t buffer_end = 0;
static bool shift_pressed = false;
static bool caps_lock = false;

// Keyboard map for non-shifted keys
static const char keyboard_map[128] = {
    0,   // Error code
    27,  // Escape
    '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=',
    '\b', // Backspace
    '\t', // Tab
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0,   // Control
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0,   // Left shift
    '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',
    0,   // Right shift
    '*', // Numeric keypad
    0,   // Alt
    ' ', // Space
    0,   // Caps lock
    0,   // F1
    0,   // F2-F10 omitted for brevity
};

// Keyboard map for shifted keys
static const char keyboard_map_shifted[128] = {
    0,
    27,
    '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+',
    '\b',
    '\t',
    'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0,
    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',
    0,
    '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?',
    0,
    '*',
    0,
    ' ',
    0,
    0,
};

static void keyboard_buffer_put(char c) {
    uint16_t next = (buffer_end + 1) % KEYBOARD_BUFFER_SIZE;
    if (next != buffer_start) {
        keyboard_buffer[buffer_end] = c;
        buffer_end = next;
    }
}

void keyboard_handler(uint8_t scancode) {
    if (scancode & 0x80) {  // Key release
        scancode &= 0x7F;   // Clear release bit
        if (scancode == KEY_LSHIFT || scancode == KEY_RSHIFT) {
            shift_pressed = false;
        }
        return;
    }

    // Key press
    if (scancode == KEY_LSHIFT || scancode == KEY_RSHIFT) {
        shift_pressed = true;
        return;
    }

    if (scancode == KEY_CAPS) {
        caps_lock = !caps_lock;
        return;
    }

    char c;
    if (shift_pressed) {
        c = keyboard_map_shifted[scancode];
    } else {
        c = keyboard_map[scancode];
    }

    if (caps_lock && c >= 'a' && c <= 'z') {
        c = c - 'a' + 'A';
    }

    if (c) {
        keyboard_buffer_put(c);
    }
}

void keyboard_handler_wrapper(struct interrupt_frame* frame) {
    (void)frame;
    uint8_t scancode = inb(0x60);  // Read from keyboard port
    keyboard_handler(scancode);
}

char keyboard_getchar(void) {
    while (buffer_start == buffer_end) {
        // Wait for character
        halt();
    }
    char c = keyboard_buffer[buffer_start];
    buffer_start = (buffer_start + 1) % KEYBOARD_BUFFER_SIZE;
    return c;
}

bool keyboard_has_char(void) {
    return buffer_start != buffer_end;
}

bool keyboard_available(void) {
    return buffer_start != buffer_end;
}

void keyboard_init(void) {
    register_interrupt_handler(IRQ_KEYBOARD, keyboard_handler_wrapper);  // IRQ1 = 33
    buffer_start = buffer_end = 0;
    shift_pressed = false;
    caps_lock = false;
}