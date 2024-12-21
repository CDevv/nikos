#include <filesystem/msdospart.h>
#include <common/screen.h>

using namespace nikos;
using namespace nikos::common;
using namespace nikos::drivers;
using namespace nikos::filesystem;

void MSDOSPartitionTable::ReadPartitions(ATA* hd)
{
    MasterBootRecord mbr;

    hd->Read28(0, (uint8_t*)&mbr, sizeof(MasterBootRecord));

    for(int i = 0x1BE; i <= 0x01FF; i++)
    {
        Screen::PrintHex(((uint8_t*)&mbr)[i]);
        Screen::Print(" ");
    }
    Screen::Print("\n");

    if (mbr.magicnumber != 0xAA55)
    {
        Screen::Print("Illegal MBR.");
        return;
    }
    
    for (int i = 0; i < 4; i++)
    {
        if (mbr.primaryPartition[i].partition_id == 0x00)
        {
            continue;
        }
        
        Screen::Print(" Partition ");
        Screen::PrintHex(i & 0xFF);

        if (mbr.primaryPartition[i].bootable == 0x80)
        {
            Screen::Print(" bootable. Type: ");
        }
        else
        {
            Screen::Print(" non-bootable. Type: ");
        }
        
        Screen::PrintHex(mbr.primaryPartition[i].partition_id);

        //ReadBiosBlock
    }   
}