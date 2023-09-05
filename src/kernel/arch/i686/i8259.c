#include "i8259.h"
#include "io.h"
#include "../../drivers/pic.h"
#include <stdbool.h>
#include <stdint.h>

void i8259_PIC_Config(uint8_t offsetP1, uint8_t offsetP2, bool auto_eoi);
void i8259_PIC_Disable();
void i8259_PIC_EOI(int irq);
void i8259_PIC_Mask(int irq);
void i8259_PIC_Unmask(int irq);
void i8259_PIC_Set_Mask(uint16_t mask);
uint16_t i8259_PIC_Get_IRQ_In_Service_R();
uint16_t i8259_PIC_Get_IRQ_Req_R();

enum {
    PIC_ICW1_ENABLE_ICW4 = 0x01,
    PIC_ICW1_ENABLE_SINGLE = 0x2,
    PIC_ICW1_ENABLE_LEVEL = 0x08,
    PIC_ICW1_INIT = 0x10
} PIC_ICW1;

enum {
    PIC_ICW4_8086 = 0x1,
    PIC_ICW4_AUTO_EOI = 0x2,
    PIC_ICW4_BUFFER_MASTER = 0x4,
    PIC_ICW4_BUFFER_SLAVE = 0x0,
    PIC_ICW4_BUFFERED = 0x8,
    PIC_ICW4_SFNM = 0x10
} PIC_ICW4;

static uint16_t PIC_Mask = 0xFFFF;
static bool PIC_Auto_EOI = false;

void i8259_PIC_Config(uint8_t offsetP1, uint8_t offsetP2, bool auto_eoi) {
    i8259_PIC_Set_Mask(0xFFFF);
    x86_outb(0x20, PIC_ICW1_ENABLE_ICW4 | PIC_ICW1_INIT);
    i8259_iowait();
    x86_outb(0xA0, PIC_ICW1_ENABLE_ICW4 | PIC_ICW1_INIT);
    i8259_iowait();
    x86_outb(0x21, offsetP1);
    i8259_iowait();
    x86_outb(0xA1, offsetP2);
    i8259_iowait();
    x86_outb(0x21, 0x4);
    i8259_iowait();
    x86_outb(0xA1, 0x2);
    i8259_iowait();
    uint8_t icw4 = auto_eoi ? PIC_ICW4_8086 | PIC_ICW4_AUTO_EOI : PIC_ICW4_8086;
    x86_outb(0x21, icw4);
    i8259_iowait();
    x86_outb(0xA1, icw4);
    i8259_iowait();
    i8259_PIC_Set_Mask(0xFFFF);
}

void i8259_PIC_EOI(int irq) {
    if (irq >=8) x86_outb(0xA0, 0x20);
    x86_outb(0x20, 0x20);
}

void i8259_PIC_Disable() {
    i8259_PIC_Set_Mask(0xFFFF);
}

void i8259_PIC_Set_Mask(uint16_t mask) {
    PIC_Mask = mask;
    x86_outb(0x21, PIC_Mask & 0xFF);
    i8259_iowait();
    x86_outb(0xA1, PIC_Mask >> 8);
    i8259_iowait();
}

uint16_t i8259_PIC_Get_Mask() {
    return x86_inb(0x21) | (x86_inb(0xA1) << 8);
}

void i8259_PIC_Mask(int irq) {
    i8259_PIC_Set_Mask(PIC_Mask | (1 << irq));
}

void i8259_PIC_Unmask(int irq) {
    i8259_PIC_Set_Mask(PIC_Mask & ~(1 << irq));
}

uint16_t i8259_PIC_Get_IRQ_Req_R() {
    x86_outb(0x20, 0x0A);
    x86_outb(0xA0, 0x0A);
    return (x86_inb(0xA0) | (x86_inb(0xA0) << 8));
}

uint16_t i8259_PIC_Get_IRQ_In_Service_R() {
    x86_outb(0x20, 0x0B);
    x86_outb(0xA0, 0x0B);
    return (x86_inb(0xA0) | (x86_inb(0xA0) << 8));
}

bool i8259_Probe() {
    i8259_PIC_Disable();
    i8259_PIC_Set_Mask(0x1447);
    return i8259_PIC_Get_Mask() == 0x1447;
}

static const PICDriver PICD = {
    .name = "I8259 PIC Driver",
    .probe = &i8259_Probe,
    .sendEOI = &i8259_PIC_EOI,
    .mask = &i8259_PIC_Mask,
    .unmask = &i8259_PIC_Unmask,
    .init = &i8259_PIC_Config,
    .disable = &i8259_PIC_Disable
};

const PICDriver* i8259_GetDriver() {
    return &PICD;
}
