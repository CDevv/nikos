#ifndef NIKOS_FILESYSTEM_MSDOSPART_H

#define NIKOS_FILESYSTEM_MSDOSPART_H

#include <common/types.h>
#include <drivers/ata.h>

using namespace nikos;
using namespace nikos::common;
using namespace nikos::drivers;

namespace nikos
{
    namespace filesystem
    {
        struct PartitionTableEntry
        {
            uint8_t bootable;

            uint8_t start_head;
            uint8_t start_sector : 6;
            uint16_t start_cylinder : 10;

            uint8_t partition_id;

            uint8_t end_head;
            uint8_t end_sector : 6;
            uint16_t end_cylinder : 10;

            uint32_t start_lba;
            uint32_t length;
        } __attribute__((packed));

        struct MasterBootRecord
        {
            uint8_t bootloader[440];
            uint32_t signature;
            uint16_t unused;

            PartitionTableEntry primaryPartition[4];

            uint16_t magicnumber;
        } __attribute__((packed));

        class MSDOSPartitionTable
        {
        public:
            static void ReadPartitions(ATA *hd);
            static MasterBootRecord ReadMasterBootRecord(ATA *hd);
        };
    } // namespace filesystem
} // namespace nikos

#endif