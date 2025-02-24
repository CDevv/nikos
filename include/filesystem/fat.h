#ifndef NIKOS_FILESYSTEM_FAT_H

#define NIKOS_FILESYSTEM_FAT_H

#include <common/types.h>
#include <drivers/ata.h>
#include <filesystem/msdospart.h>

using namespace nikos;
using namespace nikos::common;
using namespace nikos::drivers;

namespace nikos
{
    namespace filesystem
    {
        struct BiosParameterBlock32
        {
            uint8_t jump[3];
            uint8_t softName[8];
            uint16_t bytesPerSector;
            uint8_t sectorsPerCluster;
            uint16_t reservedSectors;
            uint8_t fatCopies;
            uint16_t rootDirEntries;
            uint16_t totalSectors;
            uint8_t mediaType;
            uint16_t fatSectorCount;
            uint16_t sectorsPerTrack;
            uint16_t headCount;
            uint32_t hiddenSectors;
            uint32_t totalSectorCount;

            uint32_t tableSize;
            uint16_t extFlags;
            uint16_t fatVersion;
            uint32_t rootCluster;
            uint16_t fatInfo;
            uint16_t backupSector;
            uint8_t reserved0[12];
            uint8_t driveNumber;
            uint8_t reserved1;
            uint8_t bootSignature;
            uint32_t volumeId;
            uint8_t volumeLabel[11];
            uint8_t fatTypeLabel[8];
        } __attribute__((packed));

        struct DirectoryEntry32
        {
            uint8_t name[8];
            uint8_t ext[3];
            uint8_t attributes;
            uint8_t reserved;
            uint8_t cTimeTenth;
            uint16_t cTime;
            uint16_t cDate;
            uint16_t aTime;
            uint16_t firstClusterHi;
            uint16_t wTime;
            uint16_t wDate;
            uint16_t firstClusterLow;
            uint32_t size;
        } __attribute__((packed));

        struct DirectoryEntry
        {
            char *name;
            bool readOnly;
            bool isDirectory;
            uint32_t size;
            uint32_t inode;
        };

        struct Directory
        {
            size_t childrenCount;
            DirectoryEntry *children;
        };       

        struct NameExt
        {
            uint8_t name[8];
            uint8_t ext[3];
        } __attribute__((packed));

        struct LongFileNameEntry
        {
            uint8_t sequenceNum;
            uint16_t chars1[5];
            uint8_t attribute;
            uint8_t type;
            uint8_t checksum;
            uint8_t chars2[6];
            uint16_t reserved;
            uint16_t chars3[2];
        } __attribute__((packed));   

        class FileAllocationTable32
        {
        protected:
            ATA *hd;
            uint32_t *fileAllocationTable;
            filesystem::PartitionTableEntry partitionTable;
            uint32_t partitionOffset;
            BiosParameterBlock32 bpb;
            uint32_t clusterSize;
            uint32_t fatStart;
            uint32_t fatSize;
            uint32_t dataStart;
            uint32_t rootStart;

        public:
            FileAllocationTable32(ATA *hd, int partitionNum);
            ~FileAllocationTable32();
            void ReadBiosParameterBlock();
            void ReadCluster(int32_t SIZE, uint32_t nextFileCluster);
            DirectoryEntry32 FindCluster(char *path);
            void ReadFile(char *fileName);
            void ReadFileInfo(char *fileName);
            void ListEntries();
        };

        void ReadBiosBlock(ATA *hd, uint32_t partitionOffset);
        void ReadFile(ATA *hd, uint32_t partitionOffset, char *fileName);
    } // namespace filesystem
} // namespace nikos

#endif