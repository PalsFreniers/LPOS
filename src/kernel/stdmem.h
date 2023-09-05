#pragma once
#include <stdint.h>

#define MEMORY_MIN 0x00000500
#define MEMORY_MAX 0x00080000

#define MEMORY_FAT_ADDRESS ((void*)0x20000)
#define MEMORY_FAT_SIZE 0x00010000

void* memcpy(void* dst, const void* src, uint16_t num);
void* memset(void* ptr, int value, uint16_t num);
int memcmp(const void* ptr1, const void* ptr2, uint16_t num);
