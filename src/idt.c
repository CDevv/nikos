#include <idt.h>

#define IDT_ENTRIES 256

static IDTEntry idt[IDT_ENTRIES] = {0}; 
static IDTRegister idtr;

void setIDTGate(int index, uint64_t handler, uint8_t flags)
{
    idt[index].isr_low = (uint16_t)handler;
    idt[index].kernel_cs = 40;
    idt[index].ist = 0;
    idt[index].attributes = flags;
    idt[index].isr_mid = (uint16_t)(handler >> 16);
    idt[index].isr_high = (uint32_t)(handler >> 32);
    idt[index].reserved = 0;
}

void initiateIDT()
{
    idtr.base = &idt;
    idtr.limit = IDT_ENTRIES * sizeof(IDTEntry) - 1;

    asm volatile("lidt %0" ::"m"(idtr) : "memory");
}