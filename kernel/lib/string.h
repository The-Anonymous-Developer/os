#ifndef STRING_H
#define STRING_H

#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>

// Memory functions
void* memcpy(void* dest, const void* src, size_t n);
void* memset(void* s, int c, size_t n);
int memcmp(const void* s1, const void* s2, size_t n);

// String manipulation functions
char* strcpy(char* dest, const char* src);
char* strncpy(char* dest, const char* src, size_t n);
size_t strlen(const char* str);
int strcmp(const char* s1, const char* s2);

// String formatting functions
int sprintf_simple(char* str, const char* format, ...);

#endif // STRING_H