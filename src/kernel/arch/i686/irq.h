#pragma once

#include "isr.h"

typedef void (*IRQHandler)(Registers* r);

void i686_IRQ_Init();
void i686_IRQ_Register_Handler(int irq, IRQHandler h);

