#pragma once

#include "disk.h"
#include "types.h"
#include "stdmem.h"
#include "stdio.h"

struct Partition {
    struct Disk *disk;
    u32 offset;
    u32 size;
};

void MBRDetectPartition(struct Partition *p, struct Disk *d, void *part);

bool PartitionReadSector(struct Partition *disk, u32 lba, u8 sectors, void *dataOut);
