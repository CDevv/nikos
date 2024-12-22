#ifndef NIKOS_MEMORYMANAGEMENT_H

#define NIKOS_MEMORYMANAGEMENT_H

#include <common/types.h>

using namespace nikos;
using namespace nikos::common;

namespace nikos
{
    struct MemoryChunk
    {
        MemoryChunk *next;
        MemoryChunk *prev;
        bool allocated;
        size_t size;
    };

    class MemoryManager
    {
    protected:
        MemoryChunk *first;

    public:
        static MemoryManager *activeMemoryManager;
        MemoryManager(size_t start, size_t size);
        ~MemoryManager();

        static void *malloc(size_t size);
        static void free(void *ptr);
    };
} // namespace nikos

#endif