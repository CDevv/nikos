#include <filesystem/fat.h>
#include <common/screen.h>
#include <memorymanagement.h>

using namespace nikos;
using namespace nikos::common;
using namespace nikos::drivers;
using namespace nikos::filesystem;

const uint8_t DIRENT_READONLY = 0x01;
const uint8_t DIRENT_HIDDEN = 0x02;
const uint8_t DIRENT_SYSTEM = 0x04;
const uint8_t DIRENT_VOLUME = 0x08;
const uint8_t DIRENT_DIRECTORY = 0x10;
const uint8_t DIRENT_ARCHIVE = 0x20;
const uint8_t LONG_FILE_NAME = 0x0F;

void filesystem::ReadBiosBlock(ATA *hd, uint32_t partitionOffset)
{
    BiosParameterBlock32 bpb;

    //hd->Read28(partitionOffset, (uint8_t *)&bpb, sizeof(BiosParameterBlock32));
    hd->ReadSectors(1, (uint8_t*)&bpb, partitionOffset);

    Screen::Print("Sectors per cluster: ");
    Screen::PrintHex(bpb.sectorsPerCluster);
    Screen::Print("\n");

    uint32_t fatStart = partitionOffset + bpb.reservedSectors;
    uint32_t fatSize = bpb.tableSize;

    uint32_t dataStart = fatStart + fatSize * bpb.fatCopies;
    uint32_t rootStart = dataStart + bpb.sectorsPerCluster * (bpb.rootCluster - 2);

    DirectoryEntry32 dirent[16];
    hd->Read28(rootStart, (uint8_t *)&dirent[0], 16 * sizeof(DirectoryEntry32));

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

        char *foo = "        ";
        for (int j = 0; j < 8; j++)
        {
            foo[j] = dirent[i].name[j];
        }
        Screen::Print(foo);

        char *bar = "   \n";
        for (int j = 0; j < 3; j++)
        {
            bar[j] = dirent[i].ext[j];
        }
        Screen::Print(bar);

        if ((dirent[i].attributes & 0x10) == 0x10)
        {
            // directory
            continue;
        }

        uint32_t firstFileCluster = ((uint32_t)dirent[i].firstClusterHi) << 16 | ((uint32_t)dirent[i].firstClusterLow);

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
                hd->Read28(fileSector + sectorOffset, buffer, 512);

                buffer[SIZE > 512 ? 512 : SIZE] = '\0';
                Screen::Print((char *)buffer);

                if (++sectorOffset > bpb.sectorsPerCluster)
                {
                    break;
                }
            }

            uint32_t fatSectorCountForCurrentCluster = nextFileCluster / (512 / sizeof(uint32_t));
            hd->Read28(fatStart + fatSectorCountForCurrentCluster, fatBuffer, 512);

            uint32_t fatOffsetInSectorForCurrentCluster = nextFileCluster % (512 / sizeof(uint32_t));
            nextFileCluster = ((uint32_t *)&fatBuffer)[fatOffsetInSectorForCurrentCluster] & 0x0FFFFFFF;
        }
    }
}

FileAllocationTable32::FileAllocationTable32(ATA *hd, int partitionNum)
{
    this->hd = hd;
    this->partitionTable = partitionTable;
    this->partitionOffset = partitionOffset;

    this->partitionTable = MSDOSPartitionTable::ReadMasterBootRecord(hd).primaryPartition[partitionNum];
    this->partitionOffset = partitionTable.start_lba;

    ReadBiosParameterBlock();
}

FileAllocationTable32::~FileAllocationTable32()
{
}

void FileAllocationTable32::ReadBiosParameterBlock()
{
    hd->Read28(partitionOffset, (uint8_t *)&this->bpb, sizeof(BiosParameterBlock32));

    uint32_t fatStart = partitionOffset + bpb.reservedSectors;
    uint32_t fatSize = bpb.tableSize;

    uint32_t dataStart = fatStart + fatSize * bpb.fatCopies;
    uint32_t rootStart = dataStart + bpb.sectorsPerCluster * (bpb.rootCluster - 2);

    this->fatStart = fatStart;
    this->fatSize = fatSize;
    this->dataStart = dataStart;
    this->rootStart = rootStart;
    this->clusterSize = bpb.bytesPerSector * bpb.sectorsPerCluster;

    fileAllocationTable = (uint32_t *)malloc(fatSize);
    // hd->Read28(partitionOffset, (uint8_t*)&this->fileAllocationTable, 512);
}

void FileAllocationTable32::ReadCluster(int32_t SIZE, uint32_t nextFileCluster)
{
    uint8_t buffer[513];
    uint8_t fatBuffer[513];

    while (SIZE > 0)
    {
        uint32_t fileSector = dataStart + bpb.sectorsPerCluster * (nextFileCluster - 2);
        int sectorOffset = 0;

        for (; SIZE > 0; SIZE -= 512)
        {
            hd->Read28(fileSector + sectorOffset, buffer, 512);

            buffer[SIZE > 512 ? 512 : SIZE] = '\0';
            Screen::Print((char *)buffer);

            if (++sectorOffset > bpb.sectorsPerCluster)
            {
                break;
            }
        }

        uint32_t fatSectorCountForCurrentCluster = nextFileCluster / (512 / sizeof(uint32_t));
        hd->Read28(fatStart + fatSectorCountForCurrentCluster, fatBuffer, 512);

        uint32_t fatOffsetInSectorForCurrentCluster = nextFileCluster % (512 / sizeof(uint32_t));
        nextFileCluster = ((uint32_t *)&fatBuffer)[fatOffsetInSectorForCurrentCluster] & 0x0FFFFFFF;
    }
}

char streq(const char *string1, const char *string2, uint32_t max_chars)
{
    for (int i = 0; i < max_chars; i++)
    {
        if (!string1[i] && !string2[i])
        {
            return 1;
        }
        else if (string1[i] && string2[i])
        {
            if (string1[i] != string2[i])
            {
                return 0;
            }
        }
        else
        {
            return 0;
        }
    }

    return 1;
}

void ParseLongFileName(char *entry, char *filename)
{
    LongFileNameEntry *longFileNameEntry = (LongFileNameEntry *)entry;

    Screen::Print((char *)longFileNameEntry->chars1);
    Screen::Print((char *)longFileNameEntry->chars2);
    Screen::Print((char *)longFileNameEntry->chars3);
}

DirectoryEntry32 FileAllocationTable32::FindCluster(char *path)
{
    ReadBiosParameterBlock();

    DirectoryEntry32 result;
    DirectoryEntry32 dirent[16];
    hd->Read28(rootStart, (uint8_t *)&dirent[0], 16 * sizeof(DirectoryEntry32));

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

        if ((dirent[i].attributes & 0x10) == 0x10)
        {
            // directory
            continue;
        }

        // char *longFileName = nullptr;
        // ParseLongFileName((char*)&dirent[i], longFileName);

        if (streq(path, (char *)dirent[i].name, 11))
        {
            result = dirent[i];
        }
    }

    return result;
}

void FileAllocationTable32::ReadFile(char *fileName)
{
    DirectoryEntry32 entry = FindCluster(fileName);

    uint32_t firstFileCluster = ((uint32_t)entry.firstClusterHi) << 16 | ((uint32_t)entry.firstClusterLow);

    int32_t SIZE = entry.size;
    int32_t nextFileCluster = firstFileCluster;

    ReadCluster(SIZE, nextFileCluster);
}

void FileAllocationTable32::ReadFileInfo(char *fileName)
{
    DirectoryEntry32 entry = FindCluster(fileName);

    Screen::Print("===");
    char *foo = "        ";
    for (int j = 0; j < 8; j++)
    {
        foo[j] = entry.name[j];
    }
    Screen::Print(foo);

    char *bar = "   ";
    for (int j = 0; j < 3; j++)
    {
        bar[j] = entry.ext[j];
    }
    Screen::Print(bar);
    Screen::Print("===\n");

    uint16_t cDateYear = entry.cDate & ((1 << 7) - 1);
    uint16_t cDateMonth = (entry.cDate >> 7) & ((1 << 4) - 1);
    uint16_t cDateDay = (entry.cDate >> (4 + 7)) & ((1 << 5) - 1);

    Screen::Print("Creation date: ");
    Screen::PrintInt(cDateDay);
    Screen::Print("/");
    Screen::PrintInt(cDateMonth);
    Screen::Print("/");
    Screen::PrintInt(cDateYear);
    Screen::Print("\n");

    uint16_t cTimeHour = entry.cTime & ((1 << 5) - 1);
    uint16_t cTimeMins = (entry.cTime >> 5) & ((1 << 6) - 1);

    Screen::Print("Creation time: ");
    Screen::PrintInt(cTimeHour);
    Screen::Print(":");
    Screen::PrintInt(cTimeMins);

    Screen::Print("\n");
    Screen::Print("Size in bytes: ");
    Screen::PrintInt(entry.size);
}

void FileAllocationTable32::ListEntries()
{
    DirectoryEntry32 result;
    DirectoryEntry32 dirent[16];
    hd->Read28(rootStart, (uint8_t *)&dirent[0], 16 * sizeof(DirectoryEntry32));

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

        VGAColor foreground = VGA_COLOR_WHITE;

        if ((dirent[i].attributes & DIRENT_DIRECTORY) == DIRENT_DIRECTORY)
        {
            foreground = VGA_COLOR_LIGHT_CYAN;
        }

        char *foo = "        ";
        for (int j = 0; j < 8; j++)
        {
            foo[j] = dirent[i].name[j];
        }
        Screen::Print(foo, foreground);

        char *bar = "   \n";
        for (int j = 0; j < 3; j++)
        {
            bar[j] = dirent[i].ext[j];
        }
        Screen::Print(bar);
    }
}
