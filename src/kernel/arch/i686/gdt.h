#pragma once

#define I686_GDT_CODE_SEGMENT 0x08
#define I686_GDT_DATA_SEGMENT 0x10

void i686_GDT_Init();
