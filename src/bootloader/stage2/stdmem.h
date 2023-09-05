#pragma once

#include "types.h"

#define MEMORY_MIN 0x00000500
#define MEMORY_MAX 0x00080000

#define MEMORY_FAT_ADDRESS ((void*)0x20000)
#define MEMORY_FAT_SIZE    0x00010000

#define MEMORY_LOAD_KERNEL ((void*)0x30000)
#define MEMORY_LOAD_SIZE   0x00010000

#define MEMORY_VESA_INFO ((void*)0x30000)
#define MEMORY_MODE_INFO ((void*)0x40000)

#define MEMORY_KERNEL_ADDRESS ((void*)0x100000)

#define SEG(so) ((u32)so >> 16)
#define OFF(so) ((u32)so & 0xFFFF)
#define LINEAR(so) ((SEG(so) << 4) + OFF(so))

void* memcpy(void* dst, const void* src, u16 num);
void* memset(void* ptr, int value, u16 num);
int memcmp(const void* ptr1, const void* ptr2, u16 num);
