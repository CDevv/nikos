#include <memorymanagement.h>

using namespace nikos;
using namespace nikos::common;

constexpr uint32_t EndOfKernel = 8 * 1024 * 1024;

uint32_t checksum(MemoryChunk *chunk)
{
    return 0xDEADBEEF + (uint32_t)chunk->prev + (uint32_t)chunk->next + chunk->allocated + chunk->size;
}

MemoryManager *MemoryManager::activeMemoryManager = 0;

MemoryManager::MemoryManager(size_t start, size_t size)
{
    activeMemoryManager = this;
    nextFreeChunk = nullptr;
}

MemoryManager::~MemoryManager()
{
    if (activeMemoryManager == this)
    {
        activeMemoryManager = 0;
    }
}

MemoryChunk *MemoryManager::InsertChunk(MemoryChunk *prev, size_t offset)
{
    MemoryChunk *chunk = (MemoryChunk *)((char *)prev + sizeof(MemoryChunk) + offset);
    chunk->size = prev->size - sizeof(MemoryChunk) - offset;
    chunk->allocated = 0;
    chunk->prev = prev;
    chunk->prev->next = chunk;

    prev->size = offset + sizeof(MemoryChunk);
    prev->checksum = checksum(prev);

    chunk->checksum = checksum(chunk);
    chunk->next->checksum = checksum(chunk->next);

    return chunk;
}

void MemoryManager::DeleteChunk(MemoryChunk *chunk)
{
    if (chunk->prev == chunk || chunk->next == chunk)
    {
        return;
    }

    if (chunk->allocated)
    {
        return;
    }

    chunk->prev->size += chunk->size;
    chunk->prev->next = chunk->next;
    chunk->next->prev = chunk->prev;

    chunk->prev->checksum = checksum(chunk->prev);
    chunk->next->checksum = checksum(chunk->next);
}

MemoryChunk *MemoryManager::FindFreeAllocation(size_t request)
{
    if (!nextFreeChunk)
    {
        return nullptr;
    }

    if (!nextFreeChunk->allocated && nextFreeChunk->size > request + sizeof(MemoryChunk))
    {
        return nextFreeChunk;
    }

    MemoryChunk *current = nextFreeChunk->next;
    while (current != nextFreeChunk)
    {
        if (current->checksum != checksum(current))
        {
            return nullptr;
        }

        if (!current->allocated && current->size > request + sizeof(MemoryChunk))
        {
            return current;
        }
        else
        {
            current = current->next;
        }
    }

    return nullptr;
}

uint8_t MemoryManager::InitAllocator(void *bottom, void *top)
{
    if ((uint32_t)top < EndOfKernel - 4 * sizeof(MemoryChunk))
    {
        return 1;
    }

    if (EndOfKernel > (uint32_t)bottom)
    {
        bottom = (void *)EndOfKernel;
    }

    if (!nextFreeChunk)
    {
        nextFreeChunk = (MemoryChunk *)bottom;
        nextFreeChunk->allocated = 0;
        nextFreeChunk->prev = nextFreeChunk;
        nextFreeChunk->next = nextFreeChunk;
        nextFreeChunk->size = (size_t)top - (size_t)bottom;
        nextFreeChunk->checksum = checksum(nextFreeChunk);
    }
    else
    {
        MemoryChunk *newChunk = (MemoryChunk *)bottom;
        newChunk->allocated = 0;
        newChunk->size = (size_t)top - (size_t)bottom;

        if (newChunk->next == newChunk)
        {
            nextFreeChunk->next = newChunk;
            nextFreeChunk->prev = newChunk;

            newChunk->next = nextFreeChunk;
            newChunk->prev = nextFreeChunk;

            newChunk->checksum = checksum(newChunk);
            nextFreeChunk->checksum = checksum(nextFreeChunk);
        }
        else
        {
            MemoryChunk *iter = nextFreeChunk;
            while ((uint32_t)iter->next > (uint32_t)iter)
            {
                if ((uint32_t)iter < (uint32_t)newChunk && (uint32_t)iter->next > (uint32_t)newChunk)
                {
                    if ((uint32_t)iter->next > (uint32_t)newChunk + newChunk->size && (uint32_t)iter + iter->size < (uint32_t)newChunk)
                    {
                        break;
                    }
                    else
                    {
                        // Attempted to map overlapping regions
                        return 1;
                    }
                }
            }

            newChunk->next = iter->next;
            newChunk->prev = iter;

            iter->next->prev = newChunk;
            iter->next = newChunk;

            if ((uint32_t)iter < (uint32_t)nextFreeChunk)
            {
                nextFreeChunk = iter;
            }
        }
    }

    return 0;
}

void nikos::memcpy(char *src, char *dest, size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        dest[i] = src[i];
    }
}

void nikos::memset(char *to_set, size_t size, char value)
{
    for (size_t i = 0; i < size; i++)
    {
        to_set[i] = value;
    }
}

void *MemoryManager::malloc(size_t size)
{
    if (activeMemoryManager == 0)
    {
        return 0;
    }

    MemoryChunk *allocation = activeMemoryManager->FindFreeAllocation(0);

    if (checksum(allocation) != allocation->checksum)
    {
        // Heap corruption detected
        return 0;
    }

    allocation->allocated = 1;

    if (allocation->size > size + 4 * sizeof(MemoryChunk))
    {
        activeMemoryManager->InsertChunk(allocation, size);
    }
    else
    {
        allocation->checksum = checksum(allocation);
    }

    if (activeMemoryManager->nextFreeChunk == allocation)
    {
        activeMemoryManager->nextFreeChunk = activeMemoryManager->FindFreeAllocation(0);
    }

    return allocation + 1;
}

void *MemoryManager::malloc_align(size_t size, uint32_t alignment)
{
    MemoryChunk *allocation = (MemoryChunk *)malloc(size + alignment + 2 * sizeof(MemoryChunk) - 2);

    activeMemoryManager->InsertChunk(allocation,
                                     (((uint32_t)allocation + 2 * sizeof(MemoryChunk)) % 2));

    MemoryChunk *alignedAllocation = allocation->next;
    alignedAllocation->allocated = 1;

    alignedAllocation->checksum = checksum(alignedAllocation);
    allocation->checksum = checksum(allocation);

    free(allocation + 1);

    return alignedAllocation + 1;
}

void MemoryManager::free(void *ptr)
{
    MemoryChunk *chunk = (MemoryChunk *)ptr - 1;

    if (chunk->checksum != checksum(chunk))
    {
        // Heap corruption
        return;
    }

    if (!chunk->allocated)
    {
        // Double free
        return;
    }

    chunk->allocated = 0;
    chunk->checksum = checksum(chunk);

    if (chunk->next == chunk)
    {
        return;
    }
    else if ((uint32_t)activeMemoryManager->nextFreeChunk > (uint32_t)chunk)
    {
        activeMemoryManager->nextFreeChunk = chunk;
    }
    else if (!chunk->prev->allocated &&
             ((uint32_t)chunk->prev + chunk->prev->size == (uint32_t)chunk))
    {
        activeMemoryManager->DeleteChunk(chunk);
    }
    else if ((uint32_t)chunk < (uint32_t)activeMemoryManager->nextFreeChunk)
    {
        activeMemoryManager->nextFreeChunk = chunk;
    }

    if ((uint32_t)chunk < (uint32_t)chunk->next && !chunk->next->allocated && (uint32_t)chunk->next + chunk->next->size == (uint32_t)chunk)
    {
        activeMemoryManager->DeleteChunk(chunk->next);
    }
}

void *MemoryManager::realloc(void *old_alloc, size_t new_size)
{
    MemoryChunk *oldHeader = (MemoryChunk *)old_alloc - 1;
    void *newAlloc = malloc(new_size);
    MemoryChunk *newHeader = (MemoryChunk *)newAlloc - 1;

    size_t oldDataSize = oldHeader->size - sizeof(MemoryChunk);
    size_t copySize = oldDataSize < new_size ? oldDataSize : new_size;
    memcpy((char *)old_alloc, (char *)newAlloc, copySize);

    free(old_alloc);

    return newAlloc;
}

MemoryStats MemoryManager::GetStats()
{
    MemoryStats stats = {0, 0, 1};

    stats.freeMemory += nextFreeChunk->size;

    MemoryChunk *current = nextFreeChunk->next;

    while (current != nextFreeChunk)
    {
        if (current->checksum != checksum(current))
        {
            // Heap corruption
            break;
        }

        if (current->allocated)
        {
            stats.allocatedMemory += current->size;
        }
        else
        {
            stats.freeMemory += current->size;
        }

        stats.numChunks++;
        current = current->next;
    }

    return stats;
}
