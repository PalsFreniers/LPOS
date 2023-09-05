#include "idt.h"
#include "utils/binutils.h"

typedef struct {
    uint16_t BaseLow;
    uint16_t SegmentSelector;
    uint8_t Reserved;
    uint8_t Flags;
    uint16_t BaseHigh;
} __attribute__((packed)) IDTEntry;

typedef struct {
    uint16_t Limit;
    IDTEntry* ptr;
} __attribute__((packed))IDTDesc;

IDTEntry IDT[256];

IDTDesc IDTD = { sizeof(IDT) - 1, IDT };

void __attribute__((cdecl)) i686_load_idt(IDTDesc* desc);
void i686_IDT_SetGate(int intNum, void* base, uint16_t segDesc, uint8_t flags) {
    IDT[intNum].BaseLow = ((uint32_t)base) & 0xFFFF;
    IDT[intNum].SegmentSelector = segDesc;
    IDT[intNum].Reserved = 0;
    IDT[intNum].Flags = flags;
    IDT[intNum].BaseHigh = ((uint32_t)base >> 16) & 0xFFFF;
}

void i686_IDT_DisableGate(int intNum) {
    FLAGS_UNSET(IDT[intNum].Flags, IDT_FLAG_PRESENT);
}

void i686_IDT_EnableGate(int intNum) {
    FLAG_SET(IDT[intNum].Flags, IDT_FLAG_PRESENT);
}

void i686_IDT_Init() {
    i686_load_idt(&IDTD);
}
