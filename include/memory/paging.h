#ifndef NIKOS_MEMORY_PAGING_H

#define NIKOS_MEMORY_PAGING_H

#include <common/types.h>

using namespace nikos;
using namespace nikos::common;

namespace nikos
{
    namespace memory
    {
        constexpr size_t PAGE_SIZE = 4096;

        enum PermissionType
        {
            KernelReadOnly = 0,
            KernelReadWrite = 2,
            UserReadOnly = 4,
            UserReadWrite = 6
        };

        class Pager
        {
        public:
            uint32_t base_page_directory[1024] __attribute__((aligned(4096)));
            uint32_t base_page_table[1024] __attribute__((aligned(4096)));

            Pager();
            ~Pager();
            void EnablePaging();
            void SetPageDirectory(uint32_t *page_directory);
            uint32_t *GetPageTableEntry(uint32_t *page_directory, void *virtual_addr);
            void MapMemoryRange(uint32_t *page_directory, uint32_t *page_table,
                                void *physical_addr, void *virtual_addr,
                                uint32_t size, PermissionType permission);
            void MapMemoryRangeOffset(uint32_t *page_directory, uint32_t *page_table,
                                      void *physical_addr, void *virtual_addr,
                                      uint32_t size, PermissionType permission);
            void UnmapMemoryRange(uint32_t *page_directory, void *virtual_addr, size_t len);
        };
    } // namespace memory

} // namespace nikos

#endif