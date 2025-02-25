#ifndef STRING_H
#define STRING_H

#include <stddef.h>
#include <stdarg.h>

// String functions
void* memcpy(void* dest, const void* src, size_t n);
void* memset(void* s, int c, size_t n);
char* strcpy(char* dest, const char* src);
char* strncpy(char* dest, const char* src, size_t n);
size_t strlen(const char* str);
int sprintf_simple(char* buffer, const char* format, ...);

#endif // STRING_H