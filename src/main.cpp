#include <common/types.h>
#include <common/screen.h>
#include <gdt.h>
#include <hardware/interrupts.h>

using namespace nikos::common;
using namespace nikos::hardware;

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
    interrupts.Activate();
    
    while (1);   
}