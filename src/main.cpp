#include <common/types.h>
#include <common/screen.h>
#include <gdt.h>
#include <hardware/interrupts.h>
#include <drivers/driver.h>
#include <drivers/keyboard.h>
#include <drivers/keyboardHandlers.h>

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

extern "C" void kernelMain(const void* multiboot_structure, unsigned int /*multiboot_magic*/)
{
    Screen::Print("Hello world!\n");
    Screen::Print("Hello again!\n", VGA_COLOR_MAGENTA, VGA_COLOR_BLACK);

    GlobalDescriptorTable gdt;
    Screen::Print("Loaded GDT!", VGA_COLOR_MAGENTA, VGA_COLOR_BLACK);

    InterruptManager interrupts(0x20, &gdt);
    DriverManager driverManager;

    CLIKeyboard kbHandler;
    Keyboard keyboard(&interrupts, &kbHandler);

    driverManager.AddDriver(&keyboard);
    driverManager.ActivateAll();

    interrupts.Activate();
    
    while (1);   
}