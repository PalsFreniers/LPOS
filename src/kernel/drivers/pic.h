#pragma once

#include "../stdio.h"
#include <stdbool.h>
#include <stdint.h>

typedef struct {
    const char* name;
    bool (*probe)();
    void (*sendEOI)(int irq);
    void (*mask)(int irq);
    void (*unmask)(int irq);
    void (*init)(uint8_t o1, uint8_t o2, bool aeoi);
    void (*disable)();
} PICDriver;
