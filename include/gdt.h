#ifndef NIKOS_GDT_H_
#define NIKOS_GDT_H_

#include <stdint.h>

typedef struct GDTEntry {
    uint16_t limit;
    uint16_t base_high;
    uint8_t base_mid;
    uint8_t access;
    uint8_t granularity;
    uint8_t base_low;
} __attribute__((packed)) GDTEntry;

typedef struct GDTPointer {
    uint16_t size;
    uint64_t base;
} __attribute__((packed)) GDTPointer;

typedef struct TSSEntry {
    uint16_t length;
    uint16_t base_low;
    uint8_t base_mid;
    uint8_t access;
    uint8_t granularity;
    uint8_t base_high;
    uint32_t base_upper32;
    uint32_t reserved;
} __attribute__((packed)) TSSEntry;

typedef struct TSSPointer {
    uint32_t unused0;
    uint64_t rsp0;
    uint64_t rsp1;
    uint64_t rsp2;
    uint64_t unused1;
    uint64_t ist1;
    uint64_t ist2;
    uint64_t ist3;
    uint64_t ist4;
    uint64_t ist5;
    uint64_t ist6;
    uint64_t ist7;
    uint64_t unused2;
    uint32_t iopb;
} __attribute__((packed)) TSSPointer;

typedef struct GDTEntries {
    GDTEntry entries[11];
    TSSEntry tss;
} __attribute__((packed)) GDTEntries;

void initiateGDT();

#endif