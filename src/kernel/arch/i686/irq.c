#include "irq.h"
#include "../../drivers/pic.h"
#include "i8259.h"
#include "io.h"
#include "utils/array.h"

IRQHandler IRQH[16];
static const PICDriver* PICD = NULL;

void i686_IRQ_Handler(Registers* r) {
    int irq = r->inte - 0x20;
    if(IRQH[irq] == NULL) printf("Unhandled Interupt %d\n", irq);
    else IRQH[irq](r);
    PICD->sendEOI(irq);
}

void i686_IRQ_Init() {
    const PICDriver* drivers[] = { i8259_GetDriver() };
    for(int i = 0; i < SIZEOF(drivers); i++) if(drivers[i]->probe()) PICD = drivers[i];
    if(PICD == NULL) {
        printf("[Warning] :: Unable to find a PIC Driver!\n");
        return;
    }
    printf("[Info] :: Founded driver name=\"%s\"\n", PICD->name);
    PICD->init(0x20, 0x28, false);
    for(int i = 0; i < 16; i++) i686_ISR_RegHandler(0x20 + i, i686_IRQ_Handler);
    x86_sti();
    PICD->unmask(0);
    PICD->unmask(1);
}

void i686_IRQ_Register_Handler(int irq, IRQHandler h) {
    IRQH[irq] = h;
}
