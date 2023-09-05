#pragma once

#include <stdarg.h>
#include "types.h"
#include "x86.h"

void clrScr();
void putChar(char c);
void putString(const char* str);
void __attribute__((cdecl)) printf(str str, ...);
void print_buffer(str msg, const void *buffer, u32 count);
void scrollBack(int lines);
