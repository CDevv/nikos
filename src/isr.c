#include <isr.h>
#include <idt.h>
#include <console.h>
#include <system.h>

extern void *isr_stub_table[];

void exception_handler()
{
    print("exception caught!\n");
    hcf();
}

void initiateISR()
{
    for (int i = 0; i < 32; i++)
    {
        setIDTGate(i, isr_stub_table[i], 0x8E);
    }  

    initiateIDT();
    asm("sti");
}