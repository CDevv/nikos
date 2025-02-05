#include <common/types.h>
#include <common/screen.h>
#include <gdt.h>
#include <hardware/interrupts.h>
#include <drivers/driver.h>
#include <drivers/keyboard.h>
#include <drivers/keyboardHandlers.h>
#include <drivers/ata.h>
#include <filesystem/msdospart.h>
#include <filesystem/fat.h>
#include <memory/paging.h>
#include <memorymanagement.h>
#include <syscalls.h>
#include <multitasking.h>

#include <multiboot.h>

using namespace nikos::common;
using namespace nikos::hardware;
using namespace nikos::drivers;

typedef void (*constructor)();
extern "C" constructor start_ctors;
extern "C" constructor end_ctors;
extern "C" void callConstructors()
{
    for(constructor* i = &start_ctors; i != &end_ctors; i++)
        (*i)();
}

void syscall(char* str)
{
    asm("int $0x80" : : "a" (4), "b" (str));
}

void taskA()
{
    while (true)
    {
        syscall("A");
    }
    
}

void taskB()
{
    while (true)
    {
        syscall("B");
    }
}

extern "C" void kernelMain(const void* multiboot_structure, unsigned int /*multiboot_magic*/)
{
    Screen::Print("Hello world!\n");
    Screen::Print("Hello again!\n", VGA_COLOR_MAGENTA, VGA_COLOR_BLACK);

    memory::Pager paging;
    GlobalDescriptorTable gdt;
    Screen::Print("Loaded GDT!", VGA_COLOR_MAGENTA, VGA_COLOR_BLACK);

    Screen::Print("\n");

    multiboot_info_t* bootinfo = (multiboot_info_t*)multiboot_structure;
    uint32_t memSizeKb = 1024 + bootinfo->mem_lower + bootinfo->mem_upper*64;

    Screen::Print("Mapping memory..\n");

    uint32_t* memUpper = (uint32_t*)(((size_t)multiboot_structure) + 8);
    size_t heap = 10*1024*1024;
    MemoryManager memoryManager(heap, (*memUpper)*1024 - heap - 10*1024);

    MemoryInfoEntry* memoryTable = (MemoryInfoEntry*)(bootinfo->mmap_addr + 4);
    uint16_t mmapEntriesCount = bootinfo->mmap_length / (sizeof(MemoryInfoEntry));
    for (int i = 0; i < mmapEntriesCount; i++)
    {
        Screen::Print("Region ");
        Screen::PrintInt(i);

        Screen::Print(" Start: ");
        Screen::PrintHex(memoryTable[i].addr_low);

        Screen::Print(" Size: ");
        Screen::PrintHex(memoryTable[i].length_low);

        Screen::Print(" Type: ");
        Screen::PrintInt(memoryTable[i].type);
        Screen::Print("\n");

        if (memoryTable[i].type == 1)
        {
            memoryManager.InitAllocator((void*)memoryTable[i].addr_low,
            (void*)(char*)memoryTable[i].addr_low + memoryTable[i].length_low);
        }
    }

    MemoryManager::malloc(1024);

    MemoryStats memoryStats = memoryManager.GetStats();

    Screen::Print("Free Memory: ");
    Screen::PrintInt(memoryStats.freeMemory);
    Screen::Print("\nAllocated Memory: ");
    Screen::PrintInt(memoryStats.allocatedMemory);
    Screen::Print("\nNumber of Chunks: ");
    Screen::PrintInt(memoryStats.numChunks);
    Screen::Print("\n");
    
    /*
    uint32_t* memUpper = (uint32_t*)(((size_t)multiboot_structure) + 8);
    size_t heap = 10*1024*1024;
    MemoryManager memoryManager(heap, (*memUpper)*1024 - heap - 10*1024);

    Screen::Print("heap: 0x");
    Screen::PrintHex((heap >> 24) & 0xFF);
    Screen::PrintHex((heap >> 16) & 0xFF);
    Screen::PrintHex((heap >> 8) & 0xFF);
    Screen::PrintHex(heap & 0xFF);
    Screen::Print("\n");

    void* allocated = MemoryManager::malloc(1024);
    Screen::Print("allocated: 0x");
    Screen::PrintHex(((size_t)allocated >> 24) & 0xFF);
    Screen::PrintHex(((size_t)allocated >> 16) & 0xFF);
    Screen::PrintHex(((size_t)allocated >> 8) & 0xFF);
    Screen::PrintHex((size_t)allocated & 0xFF);
    Screen::Print("\n");
    */

    TaskManager taskManager;

    Task t1(&gdt, &taskA);
    Task t2(&gdt, &taskB);

    //taskManager.AddTask(&t1);
    //taskManager.AddTask(&t2);

    InterruptManager interrupts(0x20, &gdt, &taskManager);
    SyscallHandler syscalls(&interrupts, 0x80);

    DriverManager driverManager;

    //CLIKeyboard kbHandler;
    //Keyboard keyboard(&interrupts, &kbHandler);

    //driverManager.AddDriver(&keyboard);
    //driverManager.ActivateAll();

    Screen::Print("ATA Primary Master: ");
    ATA ata0m(true, 0x1F0);
    ata0m.Identify();

    Screen::Print("ATA Primary Slave: ");
    ATA ata0s(false, 0x1F0);
    ata0s.Identify();

    //ata0m.Write28(0, (uint8_t*)"Niko's ata drive", 17);
    //ata0m.Flush();

    Screen::Print("\n");
    //ata0m.Read28(0, 17);

    Screen::Print("\n\n");
    //filesystem::MSDOSPartitionTable::ReadPartitions(&ata0m);
    
    filesystem::FileAllocationTable32 fat32(&ata0m, 0);

    fat32.ReadFile("FILE1.TXT");

    interrupts.Activate();
    
    while (1);   
}