#include <gdt.h>
#include <memory.h>

static GDTEntries gdt;
static GDTPointer gdtr;
static TSSPointer tss;

TSSPointer *tssPtr = &tss;

void loadGDT()
{
    asm volatile("lgdt %0\n\t"
                 "push $0x28\n\t"
                 "lea 1f(%%rip), %%rax\n\t"
                 "push %%rax\n\t"
                 "lretq\n\t"
                 "1:\n\t"
                 "mov $0x30, %%eax\n\t"
                 "mov %%eax, %%ds\n\t"
                 "mov %%eax, %%es\n\t"
                 "mov %%eax, %%fs\n\t"
                 "mov %%eax, %%gs\n\t"
                 "mov %%eax, %%ss\n\t"
                 :
                 : "m"(gdtr)
                 : "rax", "memory");
}

void loadTSS(TSSPointer *tss)
{
    size_t addr = (size_t)&tss;

    gdt.tss.base_low = (uint16_t)addr;
    gdt.tss.base_mid = (uint8_t)(addr >> 16);
    gdt.tss.access = 0x89;
    gdt.tss.granularity = 0;
    gdt.tss.base_high = (uint8_t)(addr >> 24);
    gdt.tss.base_upper32 = (uint32_t)(addr >> 32);
    gdt.tss.reserved = 0;

    asm volatile("ltr %0" : : "rm"((uint16_t)0x58) : "memory");
}

void encodeEntry(int index, uint16_t limit, uint8_t access, uint8_t granularity)
{
    gdt.entries[index].limit = limit;
    gdt.entries[index].base_high = 0;
    gdt.entries[index].base_mid = 0;
    gdt.entries[index].access = access;
    gdt.entries[index].granularity = granularity;
    gdt.entries[index].base_low = 0;
}

void encodeTSSEntry()
{
    gdt.tss.length = 104;
    gdt.tss.base_low = 0;
    gdt.tss.base_mid = 0;
    gdt.tss.access = 0x89;
    gdt.tss.granularity = 0;
    gdt.tss.base_high = 0;
    gdt.tss.base_upper32 = 0;
    gdt.tss.reserved = 0;
}

void initiateGDT()
{
    encodeEntry(0, 0, 0, 0);            // null (0)
    encodeEntry(1, 0xFFFF, 0x9A, 0);    // kernel code 16 (8)
    encodeEntry(2, 0xFFFF, 0x92, 0);    // kernel data 16 (16)
    encodeEntry(3, 0xFFFF, 0x9A, 0xCF); // kernel code 32 (24)
    encodeEntry(4, 0xFFFF, 0x92, 0xCF); // kernel data 32
    encodeEntry(5, 0, 0x9A, 0x20);      // kernel code 64 (40)
    encodeEntry(6, 0, 0x92, 0);         // kernel data 64 (48)
    gdt.entries[7] = (GDTEntry){0};     // SYSENTER (56)
    gdt.entries[8] = (GDTEntry){0};     // SYSENTER (64)
    encodeEntry(9, 0, 0xF2, 0);         // user data 64 (80)
    encodeEntry(10, 0, 0xFA, 0x20);     // user code 64 (72)
    encodeTSSEntry();    

    gdtr.size = sizeof(GDTEntries) - 1;
    gdtr.base = (uint64_t)&gdt;

    loadGDT();

    memset(&tss, 0, sizeof(TSSPointer));
    loadTSS(&tss);
}