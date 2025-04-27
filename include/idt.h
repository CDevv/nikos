#ifndef NIKOS_IDT_H_
#define NIKOS_IDT_H_

#include <stdint.h>

typedef struct IDTEntry {
    uint16_t isr_low;
    uint16_t kernel_cs;
    uint8_t ist;
    uint8_t attributes;
    uint16_t isr_mid;
    uint32_t isr_high;
    uint32_t reserved;
} __attribute__((packed)) IDTEntry;

typedef struct IDTRegister {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) IDTRegister;

void setIDTGate(int index, uint64_t handler, uint8_t flags);
void initiateIDT();

#endif