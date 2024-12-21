#include <filesystem/fat.h>
#include <common/screen.h>

using namespace nikos;
using namespace nikos::common;
using namespace nikos::drivers;
using namespace nikos::filesystem;

void filesystem::ReadBiosBlock(ATA *hd, uint32_t partitionOffset)
{
    BiosParameterBlock32 bpb;

    hd->Read28(partitionOffset, (uint8_t*)&bpb, sizeof(BiosParameterBlock32));

    Screen::Print("Sectors per cluster: ");
    Screen::PrintHex(bpb.sectorsPerCluster);
    Screen::Print("\n");

    uint32_t fatStart = partitionOffset + bpb.reservedSectors;
    uint32_t fatSize = bpb.tableSize;

    uint32_t dataStart = fatStart + fatSize*bpb.fatCopies;
    uint32_t rootStart = dataStart + bpb.sectorsPerCluster * (bpb.rootCluster - 2);

    DirectoryEntry32 dirent[16];
    hd->Read28(rootStart, (uint8_t*)&dirent[0], 16 * sizeof(DirectoryEntry32));

    for (int i = 0; i < 16; i++)
    {
        if (dirent[i].name[0] == 0x00)
        {
            break;
        }
        
        if ((dirent[i].attributes & 0x0F) == 0x0F)
        {
            continue;
        }
        
        char* foo = "        ";
        for (int j = 0; j < 8; j++)
        {
            foo[j] = dirent[i].name[j];
        }
        Screen::Print(foo);

        char* bar = "   \n";
        for (int j = 0; j < 3; j++)
        {
            bar[j] = dirent[i].ext[j];
        }
        Screen::Print(bar);

        if ((dirent[i].attributes & 0x10) == 0x10)
        {
            //directory
            continue;
        }

        uint32_t firstFileCluster = ((uint32_t)dirent[i].firstClusterHi) << 16
            | ((uint32_t)dirent[i].firstClusterLow);

        int32_t SIZE = dirent[i].size;
        int32_t nextFileCluster = firstFileCluster;
        uint8_t buffer[513];
        uint8_t fatBuffer[513];

        while (SIZE > 0)
        {
            uint32_t fileSector = dataStart + bpb.sectorsPerCluster * (nextFileCluster - 2);
            int sectorOffset = 0;

            for (; SIZE > 0; SIZE -= 512)
            {
                hd->Read28(fileSector+sectorOffset, buffer, 512);

                buffer[SIZE > 512 ? 512 : SIZE] = '\0';
                Screen::Print((char*)buffer);

                if (++sectorOffset > bpb.sectorsPerCluster)
                {
                    break;
                }
                
            } 

            uint32_t fatSectorCountForCurrentCluster = nextFileCluster / (512/sizeof(uint32_t));
            hd->Read28(fatStart + fatSectorCountForCurrentCluster, fatBuffer, 512);

            uint32_t fatOffsetInSectorForCurrentCluster = nextFileCluster % (512/sizeof(uint32_t));
            nextFileCluster = ((uint32_t*)&fatBuffer)[fatOffsetInSectorForCurrentCluster] & 0x0FFFFFFF;         
        }
        
    }
    
}