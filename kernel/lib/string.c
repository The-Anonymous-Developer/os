#include "string.h"
#include <stdint.h>
#include <stdarg.h>

// Memory functions
void* memcpy(void* dest, const void* src, size_t n) {
    uint8_t* d = (uint8_t*)dest;
    const uint8_t* s = (const uint8_t*)src;
    while (n--) {
        *d++ = *s++;
    }
    return dest;
}

void* memset(void* s, int c, size_t n) {
    uint8_t* p = (uint8_t*)s;
    while (n--) {
        *p++ = (uint8_t)c;
    }
    return s;
}

int memcmp(const void* s1, const void* s2, size_t n) {
    const uint8_t* p1 = (const uint8_t*)s1;
    const uint8_t* p2 = (const uint8_t*)s2;
    while (n--) {
        if (*p1 != *p2) {
            return *p1 - *p2;
        }
        p1++;
        p2++;
    }
    return 0;
}

// Basic string functions
char* strcpy(char* dest, const char* src) {
    char* d = dest;
    while ((*d++ = *src++) != '\0');
    return dest;
}

char* strncpy(char* dest, const char* src, size_t n) {
    size_t i;
    for (i = 0; i < n && src[i] != '\0'; i++) {
        dest[i] = src[i];
    }
    for (; i < n; i++) {
        dest[i] = '\0';
    }
    return dest;
}

size_t strlen(const char* str) {
    const char* s;
    for (s = str; *s; ++s);
    return s - str;
}

// Number conversion helper
static char* uint_to_string(uint32_t num, char* str, int base) {
    char* ptr = str, *ptr1 = str, tmp_char;
    uint32_t tmp_num;

    do {
        tmp_num = num % base;
        *ptr++ = (tmp_num < 10) ? (tmp_num + '0') : (tmp_num - 10 + 'a');
        num /= base;
    } while (num);

    *ptr-- = '\0';

    // Reverse the string
    while (ptr1 < ptr) {
        tmp_char = *ptr;
        *ptr-- = *ptr1;
        *ptr1++ = tmp_char;
    }

    return str;
}

// Simple sprintf implementation
int sprintf_simple(char* str, const char* format, ...) {
    va_list args;
    va_start(args, format);
    char* s = str;
    char temp[32];

    for (const char* fmt = format; *fmt; fmt++) {
        if (*fmt != '%') {
            *s++ = *fmt;
            continue;
        }

        fmt++;
        switch (*fmt) {
            case 's':
                strcpy(s, va_arg(args, char*));
                s += strlen(s);
                break;
            case 'd':
                uint_to_string(va_arg(args, uint32_t), temp, 10);
                strcpy(s, temp);
                s += strlen(temp);
                break;
            case 'x':
                uint_to_string(va_arg(args, uint32_t), temp, 16);
                strcpy(s, temp);
                s += strlen(temp);
                break;
            default:
                *s++ = *fmt;
                break;
        }
    }

    va_end(args);
    *s = '\0';
    return s - str;
}

int string_format(char* buffer, const char* format, ...) {
    va_list args;
    va_start(args, format);
    
    char* str = buffer;
    char num_buf[32];
    
    for (const char* fmt = format; *fmt; fmt++) {
        if (*fmt != '%') {
            *str++ = *fmt;
            continue;
        }
        
        fmt++;
        switch (*fmt) {
            case 'd': {
                int val = va_arg(args, int);
                int i = 0;
                if (val < 0) {
                    *str++ = '-';
                    val = -val;
                }
                do {
                    num_buf[i++] = '0' + (val % 10);
                    val /= 10;
                } while (val);
                while (i > 0) {
                    *str++ = num_buf[--i];
                }
                break;
            }
            case 's': {
                char* s = va_arg(args, char*);
                while (*s) {
                    *str++ = *s++;
                }
                break;
            }
        }
    }
    
    va_end(args);
    *str = '\0';
    return str - buffer;
}