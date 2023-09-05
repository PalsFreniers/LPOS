#pragma once

#include <stdint.h>

typedef struct {
    uint32_t ds;
    uint32_t edi, esi, ebp, Kesp, ebx, edx, ecx, eax;
    uint32_t inte, error;
    uint32_t eip, cs, eflags, esp, ss;
} __attribute__((packed))Registers;

typedef void (*ISRHandler)(Registers* r);

void i686_ISR_Init();
void i686_ISR_RegHandler(int i, ISRHandler h);
