#ifndef NIKOS_MEMORYMANAGEMENT_H

#define NIKOS_MEMORYMANAGEMENT_H

#include <common/types.h>

using namespace nikos;
using namespace nikos::common;

namespace nikos
{
    void memcpy(char *src, char *dest, size_t size);
    void memset(char *to_set, size_t size, char value);

    struct MemoryInfoEntry
    {
        uint32_t addr_low;
        uint32_t addr_high;
        uint32_t length_low;
        uint32_t length_height;
        uint32_t type;
        uint32_t reserved;
    } __attribute__((packed));

    struct MemoryChunk
    {
        MemoryChunk *next;
        MemoryChunk *prev;
        bool allocated;
        size_t size;
        uint32_t checksum;
    };

    struct MemoryStats
    {
        size_t freeMemory;
        size_t allocatedMemory;
        size_t numChunks;
    };

    class MemoryManager
    {
    protected:
        MemoryChunk *first;
        MemoryChunk *nextFreeChunk;
        MemoryChunk *InsertChunk(MemoryChunk* prev, size_t offset);
        void DeleteChunk(MemoryChunk* chunk);
        MemoryChunk *FindFreeAllocation(size_t request);

    public:
        static MemoryManager *activeMemoryManager;
        MemoryManager(size_t start, size_t size);
        ~MemoryManager();
        MemoryStats GetStats();
        uint8_t InitAllocator(void* bottom, void* top);
        

        static void *malloc(size_t size);
        static void *malloc_align(size_t size, uint32_t alignment);
        static void *realloc(void* old_alloc, size_t new_size);
        static void free(void *ptr);
    };

} // namespace nikos

#endif