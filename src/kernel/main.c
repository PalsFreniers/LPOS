#include <stdint.h>
#include "arch/i686/irq.h"
#include "stdio.h"
#include "stdmem.h"
#include "hal/hal.h"
#include "arch/i686/isr.h"
extern uint8_t __bss_start;
extern uint8_t __end;
extern void _init();

void irq0(Registers *r) {(void *)r;}

void __attribute__((section(".entry"))) kernel(uint16_t bootDrive) {
    memset(&__bss_start, 0, (&__end) - (&__bss_start));
    _init();
    HAL_Init();
    clrScr();
    printf("Hello KERNLPI\n");
    i686_IRQ_Register_Handler(0, irq0);
end:
    for(;;);
}
