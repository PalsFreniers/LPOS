#include "types.h"
#include "mbr.h"
#include "stdio.h"
#include "x86.h"
#include "disk.h"
#include "fat.h"
#include "stdmem.h"
#include "vbe.h"

u8 *KernlpiBuff = (u8*)MEMORY_LOAD_KERNEL;
u8 *Kernlpi = (u8*)MEMORY_KERNEL_ADDRESS;

typedef void (*KernlpiStart)();

void __attribute__((cdecl)) start(u16 bootDrive, void* bootPart) {
    clrScr();
    struct Disk disk;
    if (!DiskInit(&disk, bootDrive)) {
        printf("Disk initialisation error\r\n");
        goto end;
    }

    struct Partition part;
    MBRDetectPartition(&part, &disk, bootPart);

    if(!FAT_Initialize(&part)) {
        printf("FAT initialisation Error\r\n");
        goto end;
    }
    struct FAT_File *fd = FAT_Open(&part, "/boot/kernel.bin");
    u32 read;
    u8 *KernlpiBuffT = Kernlpi;
    while((read = FAT_Read(&part, fd, MEMORY_LOAD_SIZE, KernlpiBuff))) {
        memcpy(KernlpiBuffT, KernlpiBuff, read);
        KernlpiBuffT += read;
    }
    FAT_Close(fd);
    KernlpiStart Kern = (KernlpiStart)Kernlpi;
    /*
    VBEInfoBlock* vbe = (VBEInfoBlock*)MEMORY_VESA_INFO;
    VBEModeInfo* vbeModeInfo = (VBEModeInfo*)MEMORY_MODE_INFO;

    int MaxWidth = 0, MaxHeight = 0, MaxBpp = 0;
    u16 currentMode = 0xFFFF;

    if(VBE_GetController(vbe)) {
        u16* mode = (u16*)(vbe->VideoModePtr);
        for(; *mode != 0xFFFF; mode++) {
            if(!VBE_GetModeInfos(vbeModeInfo, *mode)) {
                printf("[Warning] :: Unable to Get Mode Infos for mode %x\n", *mode);
                continue;
            }
            bool HasFB = (vbeModeInfo->attributes & 0x90) == 0x90;
            if(HasFB) {
                if ((vbeModeInfo->width >= MaxWidth || vbeModeInfo->height >= MaxHeight) && vbeModeInfo->bpp >= MaxBpp) {
                    MaxWidth = vbeModeInfo->width;
                    MaxHeight = vbeModeInfo->height;
                    MaxBpp = vbeModeInfo->bpp;
                    currentMode = *mode;
                }
            }
        }
        if(!VBE_GetModeInfos(vbeModeInfo, currentMode)) {
            printf("That should not happen");
            goto end;
        }
        if(currentMode != 0xFFFF && VBE_SetMode(currentMode)) {
            u32* fb = (u32*)(vbeModeInfo->framebuffer);
            for(int i = 0; i < vbeModeInfo->height; i++) {
                for(int j = 0; j < vbeModeInfo->width; j++) {
                    fb[i*vbeModeInfo->pitch/4+j] = COLOR(0, 255, 0);
                }
            }
        }
    }
    else printf("Error VESA not present\n");*/
    Kern();
end:
    for(;;);
}
