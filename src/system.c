#include <system.h>
#include <stdint.h>

void hcf()
{
    for (;;)
    {
        asm("hlt");
    }
}

void cpuid(uint32_t *eax, uint32_t *ebx, uint32_t *ecx, uint32_t *edx)
{
    asm volatile(
        "cpuid \n"
        : "=a"(*eax), "=b"(*ebx), "=c"(*ecx), "=d"(*edx)
        : "a"(*eax)
        : "memory"
    );
}

uint8_t inportb(uint16_t port)
{
    uint8_t rv;
    __asm__ __volatile__("inb %1, %0" : "=a"(rv) : "dN"(port));
    return rv;
}

void outportb(uint16_t port, uint8_t data)
{
    __asm__ __volatile__("outb %1, %0" : : "dN"(port), "a"(data));
}

uint16_t inportw(uint16_t port)
{
    uint16_t result;
    __asm__("in %%dx, %%ax" : "=a"(result) : "d"(port));
    return result;
}

void outportw(uint16_t port, uint16_t data)
{
    __asm__("out %%ax, %%dx" : : "a"(data), "d"(port));
}

uint32_t inportl(uint16_t port)
{
    uint32_t result;
    __asm__ __volatile__("inl %%dx, %%eax" : "=a"(result) : "d"(port));
    return result;
}

void outportl(uint16_t port, uint32_t data)
{
    __asm__ __volatile__("outl %%eax, %%dx" : : "d"(port), "a"(data));
}