#include "fat.h"

#define SECTOR_SIZE             512
#define MAX_PATH_SIZE           256
#define MAX_FILE_HANDLES        10
#define ROOT_DIRECTORY_HANDLE   -1
#define FAT_CACHE_SIZE_SECTORS  5

struct FAT_EBR {
    u8 DriveNumber;
    u8 _Reserved;
    u8 Signature;
    u32 VolumeId;          // serial number, value doesn't matter
    u8 VolumeLabel[11];    // 11 bytes, padded with spaces
    u8 SystemId[8];
}__attribute__((packed));

struct FAT32_EBR {
    u32 sectorsPerFat;
    u16 flags;
    u16 versionNumber;
    u32 rootDirCluster;
    u16 fsInfoSector;
    u16 backupBootSector;
    u8  reserved[12];
    struct FAT_EBR EBR;
}__attribute__((packed));

struct FAT_BootSector {
    u8 BootJumpInstruction[3];
    u8 OemIdentifier[8];
    u16 BytesPerSector;
    u8 SectorsPerCluster;
    u16 ReservedSectors;
    u8 FatCount;
    u16 DirEntryCount;
    u16 TotalSectors;
    u8 MediaDescriptorType;
    u16 SectorsPerFat;
    u16 SectorsPerTrack;
    u16 Heads;
    u32 HiddenSectors;
    u32 LargeSectorCount;
    // extended boot record
    union {
        struct FAT_EBR   FAT1216;
        struct FAT32_EBR FAT32;
    };
}__attribute__((packed));

struct FAT_FileData {
    u8 Buffer[SECTOR_SIZE];
    struct FAT_File Public;
    bool Opened;
    u32 FirstCluster;
    u32 CurrentCluster;
    u32 CurrentSectorInCluster;
};

struct FAT_Data {
    union
    {
        struct FAT_BootSector BootSector;
        u8 BootSectorBytes[SECTOR_SIZE];
    } BS;
    struct FAT_FileData RootDirectory;
    struct FAT_FileData OpenedFiles[MAX_FILE_HANDLES];
    u8 FATCache[FAT_CACHE_SIZE_SECTORS*SECTOR_SIZE];
    u32 FATCachePosition;
};

static struct FAT_Data *g_Data;
static u32 g_DataSectionLba;
static u8 g_FATType;
static u32 g_TotalSectors;
static u32 g_SectorPerFat;

bool FAT_ReadBootSector(struct Partition *part) {
    return PartitionReadSector(part, 0, 1, g_Data->BS.BootSectorBytes);
}

bool FAT_ReadFat(struct Partition *part, size_t lbaIndex) {
    return PartitionReadSector(part, g_Data->BS.BootSector.ReservedSectors + lbaIndex, FAT_CACHE_SIZE_SECTORS, g_Data->FATCache);
}

void FATDetect(struct Partition *part) {
    u32 dataClusters = (g_TotalSectors - g_DataSectionLba)/g_Data->BS.BootSector.SectorsPerCluster;
    if(dataClusters < 0xFF5) g_FATType = 12;
    else if(g_Data->BS.BootSector.SectorsPerFat != 0) g_FATType = 16;
    else g_FATType = 32;
}

u32 FAT_ClusterToLba(u32 cluster) {
    return g_DataSectionLba + (cluster - 2) * g_Data->BS.BootSector.SectorsPerCluster;
}

bool FAT_Initialize(struct Partition *part) {
    g_Data = (struct FAT_Data *)MEMORY_FAT_ADDRESS;

    if (!FAT_ReadBootSector(part)) {
        printf("FAT: read boot sector failed\n");
        return false;
    }

    g_Data->FATCachePosition = 0xFFFFFFFF;

    g_TotalSectors = g_Data->BS.BootSector.TotalSectors;
    if(g_TotalSectors == 0) g_TotalSectors = g_Data->BS.BootSector.LargeSectorCount;
    
    bool isFat32 = false;
    g_SectorPerFat = g_Data->BS.BootSector.SectorsPerFat;
    if(g_SectorPerFat == 0) {
        isFat32 = true;
        g_SectorPerFat = g_Data->BS.BootSector.FAT32.sectorsPerFat;
    }

    u32 rootDirLba = 0; 
    u32 rootDirSize = 0;
    if(isFat32) {
        g_DataSectionLba = g_Data->BS.BootSector.ReservedSectors + g_SectorPerFat * g_Data->BS.BootSector.FatCount;
        rootDirLba = FAT_ClusterToLba(g_Data->BS.BootSector.FAT32.rootDirCluster);
    } 
    else {
        rootDirLba = g_Data->BS.BootSector.ReservedSectors + g_SectorPerFat * g_Data->BS.BootSector.FatCount;
        rootDirSize = sizeof(struct FAT_DirectoryEntry) * g_Data->BS.BootSector.DirEntryCount;
        u32 rootDirSectors = (rootDirSize + g_Data->BS.BootSector.BytesPerSector - 1) / g_Data->BS.BootSector.BytesPerSector;
        g_DataSectionLba = rootDirLba + rootDirSectors;
    }

    g_Data->RootDirectory.Public.Handle = ROOT_DIRECTORY_HANDLE;
    g_Data->RootDirectory.Public.IsDirectory = true;
    g_Data->RootDirectory.Public.Position = 0;
    g_Data->RootDirectory.Public.Size = sizeof(struct FAT_DirectoryEntry) * g_Data->BS.BootSector.DirEntryCount;
    g_Data->RootDirectory.Opened = true;
    g_Data->RootDirectory.FirstCluster = rootDirLba;
    g_Data->RootDirectory.CurrentCluster = rootDirLba;
    g_Data->RootDirectory.CurrentSectorInCluster = 0;

    if (!PartitionReadSector(part, rootDirLba, 1, g_Data->RootDirectory.Buffer)) {
        printf("FAT: read root directory failed\n");
        return false;
    }
    FATDetect(part);

    for (int i = 0; i < MAX_FILE_HANDLES; i++) g_Data->OpenedFiles[i].Opened = false;

    return true;
}


struct FAT_File* FAT_OpenEntry(struct Partition *part, struct FAT_DirectoryEntry *entry) {
    // find empty handle
    int handle = -1;
    for (int i = 0; i < MAX_FILE_HANDLES && handle < 0; i++)
    {
        if (!g_Data->OpenedFiles[i].Opened)
            handle = i;
    }

    // out of handles
    if (handle < 0)
    {
        printf("FAT: out of file handles\n");
        return false;
    }

    // setup vars
    struct FAT_FileData* fd = &g_Data->OpenedFiles[handle];
    fd->Public.Handle = handle;
    fd->Public.IsDirectory = (entry->Attributes & FAT_ATTRIBUTE_DIRECTORY) != 0;
    fd->Public.Position = 0;
    fd->Public.Size = entry->Size;
    fd->FirstCluster = entry->FirstClusterLow + ((u32)entry->FirstClusterHigh << 16);
    fd->CurrentCluster = fd->FirstCluster;
    fd->CurrentSectorInCluster = 0;

    if (!PartitionReadSector(part, FAT_ClusterToLba(fd->CurrentCluster), 1, fd->Buffer))
    {
        printf("FAT: Open Entry failed - read error\n");
        return false;
    }

    fd->Opened = true;
    return &fd->Public;
}

u32 FAT_NextCluster(struct Partition *part, u32 currentCluster) {
    u32 index;
    switch (g_FATType) {
    case 12:
        index = currentCluster*3/2;
        break;
    case 16:
        index = currentCluster*2;
        break;
    case 32:
        index = currentCluster*4;
        break;
    default:
        break;
    }
    u32 indexSector = index/SECTOR_SIZE;
    if(indexSector < g_Data->FATCachePosition || indexSector >= g_Data->FATCachePosition+FAT_CACHE_SIZE_SECTORS) {
        FAT_ReadFat(part, indexSector);
        g_Data->FATCachePosition = indexSector;
    }
    
    index -= (g_Data->FATCachePosition*SECTOR_SIZE);
    
    u32 nextCluster;
    switch (g_FATType) {
    case 12:
        if (currentCluster % 2 == 0) nextCluster = (*(u16*)(g_Data->FATCache + index)) & 0x0FFF;
        else nextCluster = (*(u16*)(g_Data->FATCache + index)) >> 4;
        if(nextCluster >= 0xFF8) nextCluster |= 0xFFFFF000;
        break;
    case 16:
        nextCluster = (*(u16*)(g_Data->FATCache + index));
        if(nextCluster >= 0xFFF8) nextCluster |= 0xFFFF0000;
        break;
    case 32:
        nextCluster = (*(u32*)(g_Data->FATCache + index));
        break;
    default:
        break;
    }
    return nextCluster;
}

u32 FAT_Read(struct Partition* part, struct FAT_File* file, u32 byteCount, void* dataOut)
{
    // get file data
    struct FAT_FileData* fd = (file->Handle == ROOT_DIRECTORY_HANDLE) 
        ? &g_Data->RootDirectory 
        : &g_Data->OpenedFiles[file->Handle];

    u8* u8DataOut = (u8*)dataOut;

    // don't read past the end of the file
    if (!fd->Public.IsDirectory ||(fd->Public.IsDirectory && fd->Public.Size != 0)) 
        byteCount = MIN(byteCount, fd->Public.Size - fd->Public.Position);

    while (byteCount > 0)
    {
        u32 leftInBuffer = SECTOR_SIZE - (fd->Public.Position % SECTOR_SIZE);
        u32 take = MIN(byteCount, leftInBuffer);

        memcpy(u8DataOut, fd->Buffer + fd->Public.Position % SECTOR_SIZE, take);
        u8DataOut += take;
        fd->Public.Position += take;
        byteCount -= take;

        // printf("leftInBuffer=%lu take=%lu\n", leftInBuffer, take);
        // See if we need to read more data
        if (leftInBuffer == take)
        {
            // Special handling for root directory
            if (fd->Public.Handle == ROOT_DIRECTORY_HANDLE)
            {
                ++fd->CurrentCluster;

                // read next sector
                if (!PartitionReadSector(part, fd->CurrentCluster, 1, fd->Buffer))
                {
                    printf("FAT: read error!\n");
                    break;
                }
            }
            else
            {
                // calculate next cluster & sector to read
                if (++fd->CurrentSectorInCluster >= g_Data->BS.BootSector.SectorsPerCluster)
                {
                    fd->CurrentSectorInCluster = 0;
                    fd->CurrentCluster = FAT_NextCluster(part, fd->CurrentCluster);
                }

                if (fd->CurrentCluster >= 0xFFFFFFF8)
                {
                    // Mark end of file
                    fd->Public.Size = fd->Public.Position;
                    break;
                }

                // read next sector
                if (!PartitionReadSector(part, FAT_ClusterToLba(fd->CurrentCluster) + fd->CurrentSectorInCluster, 1, fd->Buffer))
                {
                    printf("FAT: read error!\n");
                    break;
                }
            }
        }
    }
    return u8DataOut - (u8*)dataOut;
}

bool FAT_ReadEntry(struct Partition* part, struct FAT_File* file, struct FAT_DirectoryEntry* dirEntry)
{
    return FAT_Read(part, file, sizeof(struct FAT_DirectoryEntry), dirEntry) == sizeof(struct FAT_DirectoryEntry);
}

void FAT_Close(struct FAT_File* file)
{
    if (file->Handle == ROOT_DIRECTORY_HANDLE)
    {
        file->Position = 0;
        g_Data->RootDirectory.CurrentCluster = g_Data->RootDirectory.FirstCluster;
    }
    else
    {
        g_Data->OpenedFiles[file->Handle].Opened = false;
    }
}

bool FAT_FindFile(struct Partition* part, struct FAT_File* file, const str name, struct FAT_DirectoryEntry* entryOut)
{
    char fatName[12];
    struct FAT_DirectoryEntry entry;

    // convert from name to fat name
    memset(fatName, ' ', 12);
    fatName[11] = '\0';

    str ext = strsearch(name, '.');
    if (ext == NULL)
        ext = name + 11;

    for (int i = 0; i < 8 && name[i] && name + i < ext; i++)
        fatName[i] = toUpper(name[i]);

    if (ext != name + 11)
    {
        for (int i = 0; i < 3 && ext[i + 1]; i++)
            fatName[i + 8] = toUpper(ext[i + 1]);
    }
    while (FAT_ReadEntry(part, file, &entry))
    {
        if (memcmp(fatName, entry.Name, 11) == 0)
        {
            *entryOut = entry;
            return true;
        }
    }
    
    return false;
}

struct FAT_File* FAT_Open(struct Partition* part, str path)
{
    char name[MAX_PATH_SIZE];

    // ignore leading slash
    if (path[0] == '/')
        path++;

    struct FAT_File* current = &g_Data->RootDirectory.Public;

    while (*path) {
        // extract next file name from path
        bool isLast = false;
        str delim = strsearch(path, '/');
        if (delim != NULL)
        {
            memcpy(name, path, delim - path);
            name[delim - path] = '\0';
            path = delim + 1;
        }
        else
        {
            size_t len = strlen(path);
            memcpy(name, path, len);
            name[len + 1] = '\0';
            path += len;
            isLast = true;
        }
        // find directory entry in current directory
        struct FAT_DirectoryEntry entry;
        if (FAT_FindFile(part, current, name, &entry))
        {
            FAT_Close(current);

            // check if directory
            if (!isLast && (entry.Attributes & FAT_ATTRIBUTE_DIRECTORY) == 0)
            {
                printf("FAT: %s not a directory\n", name);
                return NULL;
            }

            // open new directory entry
            current = FAT_OpenEntry(part, &entry);
        }
        else
        {
            FAT_Close(current);

            printf("FAT: %s not found\n", name);
            return NULL;
        }
    }

    return current;
}
