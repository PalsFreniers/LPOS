#pragma once
#include "types.h"

void __attribute__((cdecl)) x86_outb(u16 port, u8 value);
u8 __attribute__((cdecl)) x86_inb(u16 port);
bool __attribute__((cdecl)) x86_Get_Drive_Param(u8 drive, u8* driveTypeOut, u16* cylinders, u16* sectors, u16* heads);
bool __attribute__((cdecl)) x86_Disk_Reset(u8 drive);
bool __attribute__((cdecl)) x86_Disk_Read(u8 drive, u16 c, u16 s, u16 h, u8 count, void* lowerDataOut);
int __attribute__((cdecl)) x86_Video_Get_VBE_Info(void* info);
int __attribute__((cdecl)) x86_Video_Get_VBE_Mode_Info(void* info, u16 mode);
int __attribute__((cdecl)) x86_Video_Set_VBE_Mode(u16 mode);
