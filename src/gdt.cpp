#include <gdt.h>

using namespace nikos;

void memset(char *to_set, size_t size, char value) {
  for (size_t i = 0; i < size; i++) {
    to_set[i] = value;
  }
}

constexpr uint8_t NULL_SEGMENT = 0x00;
constexpr uint8_t USER_CODE_SEGMENT = 0xFA;
constexpr uint8_t CODE_SEGMENT = 0x9A;
constexpr uint8_t USER_DATA_SEGMENT = 0xF2;
constexpr uint8_t DATA_SEGMENT = 0x92;
constexpr uint8_t TSS_SEGMENT = 0xE9;

constexpr uint8_t FOUR_KB_BLOCKS = 0x80;
constexpr uint8_t PROTECTED_MODE = 0x40;

constexpr uint8_t TSS_SELECTOR = 0x2B;

GlobalDescriptorTable::GlobalDescriptorTable()
{
    Setup();
}

void GlobalDescriptorTable::MakeEntry(GDTEntry* entry, uint64_t base, uint32_t limit, uint8_t flags, uint8_t access)
{
    entry->base_low = base & 0xFFFF;
    entry->base_mid = (base >> 16) & 0xFF;
    entry->base_high = base >> 24;

    entry->limit = limit & 0xFFFF;
    entry->flags = flags;
    entry->flags |= (limit >> 16) & 0xF;

    entry->access = access;
}

void GlobalDescriptorTable::Setup()
{
    memset((char*)&mainTSS, sizeof(TSS), 0);
    
    MakeEntry(entries, 0, 0, 0, NULL_SEGMENT);
    MakeEntry(entries + 1, 0, 0xFFFFF, FOUR_KB_BLOCKS | PROTECTED_MODE, CODE_SEGMENT);
    MakeEntry(entries + 2, 0, 0xFFFFF, FOUR_KB_BLOCKS | PROTECTED_MODE, DATA_SEGMENT);
    MakeEntry(entries + 3, 0, 0xFFFFF, FOUR_KB_BLOCKS | PROTECTED_MODE, USER_CODE_SEGMENT);
    MakeEntry(entries + 4, 0, 0xFFFFF, FOUR_KB_BLOCKS | PROTECTED_MODE, USER_DATA_SEGMENT);

    MakeEntry(entries + 5, (uint32_t)&mainTSS, sizeof(TSS), 0, TSS_SEGMENT);

    Load();
    FlushTSS();
}

void GlobalDescriptorTable::Load()
{
    GDTDescriptor descriptor;
    descriptor.address = (uint32_t)entries;
    descriptor.size = 9 * sizeof(GDTEntry) - 1;

    void *descriptorAddress = &descriptor;

    asm volatile(
        "lgdt (%0)\n"
               "jmp $0x08,$reload_data\n"
               "reload_data:\n"
               "mov $0x10, %%ax\n"
               "mov %%ax, %%ds\n"
               "mov %%ax, %%es\n"
               "mov %%ax, %%ss\n"
               "mov %%ax, %%gs\n"
               "mov %%ax, %%fs\n"
               :
               : "r"(descriptorAddress)
    );
}

//TSS
void GlobalDescriptorTable::LoadTLS(uint16_t selector)
{
    asm volatile(
        "mov %0, %%ax\n"
               "mov %%ax, %%gs\n"
               "mov %%ax, %%fs\n"
               :
               : "r"(selector)
    );
}

void GlobalDescriptorTable::SetTLS(TLSSegment &tls)
{
    MakeEntry(entries + tls.gdt_index, tls.segment_base, tls.limit, FOUR_KB_BLOCKS | PROTECTED_MODE, USER_DATA_SEGMENT);
    LoadTLS(tls.gdt_index * sizeof(GDTEntry) | 0x3);
}

void GlobalDescriptorTable::FlushTSS()
{
    entries[5].access = TSS_SEGMENT;
    asm volatile(
        "movl %0, %%eax\n"
               "ltr %%ax\n"
               :
               : "i"(TSS_SELECTOR)
    );
}