#include "io.h"

void i8259_iowait() {
    x86_outb(0x80, 0);
}

