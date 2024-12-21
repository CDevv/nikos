#include <common/types.h>
#include <common/screen.h>
#include <gdt.h>
#include <hardware/interrupts.h>
#include <drivers/driver.h>
#include <drivers/keyboard.h>
#include <drivers/keyboardHandlers.h>
#include <drivers/ata.h>
#include <filesystem/msdospart.h>
#include <syscalls.h>
#include <multitasking.h>

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

    GlobalDescriptorTable gdt;
    Screen::Print("Loaded GDT!", VGA_COLOR_MAGENTA, VGA_COLOR_BLACK);

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
    filesystem::MSDOSPartitionTable::ReadPartitions(&ata0m);

    interrupts.Activate();
    
    while (1);   
}