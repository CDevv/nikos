#ifndef NIKOS_GDT_H

#define NIKOS_GDT_H

#include <common/types.h>

using namespace nikos;
using namespace nikos::common;

namespace nikos
{
    struct GDTEntry
    {
        uint16_t limit;
        uint16_t base_low;
        uint8_t base_mid;
        uint8_t access;
        uint8_t flags;
        uint8_t base_high;
    } __attribute__((packed));

    struct GDTDescriptor
    {
        uint16_t size;
        uint32_t address;
    } __attribute__((packed));

    struct TSS
    {
        uint32_t prev_tss;
        uint32_t esp0;
        uint32_t ss0;
        uint32_t esp1;
        uint32_t ss1;
        uint32_t esp2;
        uint32_t ss2;
        uint32_t cr3;
        uint32_t eip;
        uint32_t eflags;
        uint32_t eax;
        uint32_t ecx;
        uint32_t edx;
        uint32_t ebx;
        uint32_t esp;
        uint32_t ebp;
        uint32_t esi;
        uint32_t edi;
        uint32_t es;
        uint32_t cs;
        uint32_t ss;
        uint32_t ds;
        uint32_t fs;
        uint32_t gs;
        uint32_t ldt;
        uint16_t trap;
        uint16_t iomap_base;
    } __attribute__((packed));
    
    struct TLSSegment
    {
        int32_t gdt_index;
        uint32_t segment_base; // Virtual memory address
        uint32_t limit;        // Note: limit, not length, so subtract 1
        uint32_t seg_32bits : 1;
        uint32_t contents : 1;
        uint32_t read_exec_only : 1;
        uint32_t limit_in_pages : 1;
        uint32_t useable : 1;
    } __attribute__((packed));

    class GlobalDescriptorTable
    {
        private:
            GDTEntry entries[9];
            TSS mainTSS;
            void MakeEntry(GDTEntry* entry, uint64_t base, uint32_t limit, uint8_t flags, uint8_t access);
            void Setup();
            void Load();
            void LoadTLS(uint16_t selector);
            void FlushTSS();

        public:
            GlobalDescriptorTable();
            ~GlobalDescriptorTable();
            void SetTLS(TLSSegment &tls);
    };
    
} // namespace nikos


#endif