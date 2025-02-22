#ifndef NIKOS_DRIVERS_ATA_H

#define NIKOS_DRIVERS_ATA_H

#include <hardware/port.h>

using namespace nikos;
using namespace nikos::hardware;

namespace nikos
{
    namespace drivers
    {
        class ATA
        {
        protected:
            bool master;
            Port16 dataPort;
            Port8 errorPort;
            Port8 sectorCountPort;
            Port8 lbaLowPort;
            Port8 lbaMidPort;
            Port8 lbaHiPort;
            Port8 devicePort;
            Port8 commandPort;
            Port8 controlPort;

            void EnterLba(uint32_t lba, uint8_t sectorCount);
            void BlockUntilDriveReady();

        public:
            ATA(bool master, uint16_t portBase);
            ~ATA();

            void Identify();
            void Read28(uint32_t sectorNum, uint8_t* data, int count);
            void Write28(uint32_t sectorNum, uint8_t *data, uint32_t count);
            void ReadSectors(uint32_t sectors, uint8_t *buffer, uint32_t lba);
            void WriteSectors(uint32_t sectors, uint8_t *buffer, uint32_t lba);
            void Flush();
        };
    } // namespace drivers
} // namespace nikos

#endif