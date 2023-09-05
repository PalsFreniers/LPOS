#pragma once

#include "mbr.h"
#include "stdio.h"
#include "stdmem.h"
#include "stdutils.h"
#include "stdstring.h"
#include "types.h"
 
struct FAT_DirectoryEntry {
    u8 Name[11];
    u8 Attributes;
    u8 _Reserved; 
    u8 CreatedTimeTenths;
    u16 CreatedTime;
    u16 CreatedDate;
    u16 AccessedDate; 
    u16 FirstClusterHigh;
    u16 ModifiedTime;
    u16 ModifiedDate; 
    u16 FirstClusterLow; 
    u32 Size;
} __attribute__((packed));


struct FAT_File {
    int Handle;
    bool IsDirectory;
    u32 Position;
    u32 Size;
};

enum FAT_Attributes
{
    FAT_ATTRIBUTE_READ_ONLY = 0x01,
    FAT_ATTRIBUTE_HIDDEN = 0x02,
    FAT_ATTRIBUTE_SYSTEM = 0x04,
    FAT_ATTRIBUTE_VOLUME_ID  = 0x08,
    FAT_ATTRIBUTE_DIRECTORY = 0x10,
    FAT_ATTRIBUTE_ARCHIVE = 0x20,
    FAT_ATTRIBUTE_LFN = FAT_ATTRIBUTE_READ_ONLY | FAT_ATTRIBUTE_HIDDEN | FAT_ATTRIBUTE_SYSTEM | FAT_ATTRIBUTE_VOLUME_ID
};

bool FAT_Initialize(struct Partition *part);
struct FAT_File *FAT_Open(struct Partition *part, str path);
u32 FAT_Read(struct Partition *part, struct FAT_File *file, u32 byteCount, void *dataOut);
bool FAT_ReadEntry(struct Partition *part, struct FAT_File *file, struct FAT_DirectoryEntry *dirEntry);
void FAT_Close(struct FAT_File *file);
