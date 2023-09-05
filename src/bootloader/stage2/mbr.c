#include "mbr.h"

struct MBREntry {
    u8 attr;
    u8 chsS[3];
    u8 partType;
    u8 chsE[3];
    u32 lba;
    u32 size;
};

void MBRDetectPartition(struct Partition *p, struct Disk *d, void *part) {
    p->disk = d;
    if(d->id < 0x80) {
        p->offset = 0;
        p->size = (u32)d->cylinder*(u32)d->head*(u32)d->sector;
    } else {
        struct MBREntry *entry = (struct MBREntry *)LINEAR(part);
        p->offset = entry->lba;
        p->size = entry->size;
    }
}

bool PartitionReadSector(struct Partition *p, u32 lba, u8 sectors, void *dataOut) {
    return DiskReadSector(p->disk, lba + p->offset, sectors, dataOut);
}
