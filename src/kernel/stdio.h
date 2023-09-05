#pragma once
#include <stdint.h>
#include <stddef.h>

void clrScr();
void putChar(char c);
void putString(const char* str);
void __attribute__((cdecl)) printf(const char* str, ...);
void print_buffer(const char* msg, const void* buffer, uint32_t count);
void scrollBack(int lines);
