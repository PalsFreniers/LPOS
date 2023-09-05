#include "hal.h"
#include "../arch/i686/gdt.h"
#include "../arch/i686/idt.h"
#include "../arch/i686/isr.h"
#include "../arch/i686/irq.h"

void HAL_Init() {
    i686_GDT_Init();
    i686_IDT_Init();
    i686_ISR_Init();
    i686_IRQ_Init();
}
