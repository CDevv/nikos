#ifndef NIKOS_SYS_H_

#define NIKOS_SYS_H_

// Halt and catch fire
void hcf();
void cpuid(uint32_t *eax, uint32_t *ebx, uint32_t *ecx, uint32_t *edx);

uint8_t inportb(uint16_t port);
void outportb(uint16_t port, uint8_t data);
uint16_t inportw(uint16_t port);
void outportw(uint16_t port, uint16_t data);
uint32_t inportl(uint16_t port);
void outportl(uint16_t port, uint32_t data);

#endif // NIKOS_SYS_H_