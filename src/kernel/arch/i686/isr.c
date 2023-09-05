#include "isr.h"
#include "idt.h"
#include "io.h"
#include "../../stdio.h"

ISRHandler ISRH[256];

static const char* const ISRE[] = {
    "Division by zero impossible",
    "Debug int",
    "Non-maskable Interrupt",
    "Breakpoint in debug",
    "Overflow exeption",
    "Bound Out of Range",
    "Bad Opcode",
    "Device Unavailable",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Invalid TSS",
    "Segment Absent",
    "Stack-Segment Fault",
    "General Protection Fault",
    "Page Fault",
    "",
    "x87 Floating-Point Exception",
    "Alignment Check",
    "Machine Check",
    "SIMD Floating-Point Exception",
    "Virtualization Exception",
    "Control Protection Exception ",
    "",
    "",
    "",
    "",
    "",
    "",
    "Hypervisor Injection Exception",
    "VMM Communication Exception",
    "Security Exception",
    ""
};

void i686_ISR_InitGate();

void i686_ISR_Init() {
    i686_ISR_InitGate();
    for(int i = 0; i < 256; i++) i686_IDT_EnableGate(i);
}

void __attribute__((cdecl)) i686_ISR_Handler(Registers* r) {
    if(ISRH[r->inte] != NULL) ISRH[r->inte](r);
    else if(r->inte >= 32) printf("[Warning] :: Unhandled Interupt %d!\n", r->inte);
    else {
        printf("[Error] :: Unhandled Exception %d %s\n", r->inte, ISRE[r->inte]);
        printf("\tRegisters states :\n");
        printf("\t\teax=%x ebx=%x ecx=%x edx=%x esi=%x edi=%x\n", r->eax, r->ebx, r->ecx, r->edx, r->esi, r->edi);
        printf("\t\tesp=%x ebp=%x eip=%x eflags=%x cs=%x ds=%x ss=%x\n", r->esp, r->ebp, r->eip, r->eflags, r->cs, r->ds, r->ss);
        printf("\tException informations:\n");
        printf("\t\tint=%x error=%x\n", r->inte, r->error);
        printf("__!!![[KERNEL PANIC]]!!!__\n");
        x86_kernel_panic();
    }
}

void i686_ISR_RegHandler(int i, ISRHandler h) {
    ISRH[i] = h;
    i686_IDT_EnableGate(i);
}
