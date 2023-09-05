#pragma once
#include <stdint.h>
#include <stdbool.h>

void __attribute__((cdecl)) x86_outb(uint16_t port, uint8_t value);
uint8_t __attribute__((cdecl)) x86_inb(uint16_t port);
void __attribute__((cdecl)) x86_cli();
void __attribute__((cdecl)) x86_sti();

void i8259_iowait();
void __attribute__((cdecl)) x86_kernel_panic();
