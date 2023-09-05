#pragma once

#include "types.h"
#include "stdio.h"
#include "x86.h"

struct Disk {
    u8 id;
    u16 cylinder;
    u16 sector;
    u16 head;
};

bool DiskInit(struct Disk* disk, u8 driveNumber);
bool DiskReadSector(struct Disk* disk, u32 lba, u8 sectors, void* dataOut);
