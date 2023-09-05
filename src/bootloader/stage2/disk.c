#include "disk.h"

bool DiskInit(struct Disk* disk, u8 driveNumber) {
    u8 driveType;
    u16 cylinders, sectors, heads;

    if(!x86_Get_Drive_Param(driveNumber, &driveType, &cylinders, &sectors, &heads)) return false;
    disk->id = driveNumber;
    disk->cylinder = cylinders; disk->head = heads; disk->sector = sectors;
    return true;
}

void lba_to_chs(struct Disk* disk, u32 lba, u16* cylinderOut, u16* headOut, u16* sectorOut) {
    *sectorOut = lba % disk->sector + 1;
    *cylinderOut = (lba / disk->sector) / disk->head;
    *headOut = (lba / disk->sector) % disk->head;
}

bool DiskReadSector(struct Disk* disk, u32 lba, u8 sectors, void * dataOut) {
    u16 cylinder, head, sector;
    lba_to_chs(disk, lba, &cylinder, &head, &sector);
    for (int i = 0; i < 3; i++) {
        if(x86_Disk_Read(disk->id, cylinder, sector, head, sectors, dataOut)) return true;
        x86_Disk_Reset(disk->id);
    }
    return false;
}
