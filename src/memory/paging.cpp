#include <memory/paging.h>
#include <common/screen.h>

using namespace nikos;
using namespace nikos::common;
using namespace nikos::memory;

Pager::Pager()
{
    MapMemoryRange(base_page_directory, base_page_table,
                   0, 0, (uint32_t)1024 * 1023 * 4096,
                   KernelReadWrite);

    SetPageDirectory(base_page_directory);
    EnablePaging();
}

Pager::~Pager()
{
}

void Pager::EnablePaging()
{
    asm volatile("mov %%cr0, %%ecx\n"
                 "or $0x80000001, %%ecx\n"
                 "mov %%ecx, %%cr0\n"
                 "mov %%cr3, %%ecx\n"
                 "mov %%ecx, %%cr3"
                 :
                 :
                 : "ecx");
}

void Pager::SetPageDirectory(uint32_t *page_directory)
{
    asm volatile("mov %0, %%cr3"
                 :
                 : "r"(page_directory));
}

uint32_t *Pager::GetPageTableEntry(uint32_t *page_directory, void *virtual_addr)
{
    uint32_t page_dir_index = (uint32_t)virtual_addr >> 22;
    uint32_t page_table_index = ((uint32_t)virtual_addr >> 12) & 0x3FF;
    uint32_t *page_table = (uint32_t*)(page_directory[page_dir_index] & (~0xFFF));

    if (page_table == nullptr)
    {
        return nullptr;
    }
    
    return page_table + page_table_index;
}

void Pager::MapMemoryRange(uint32_t *page_directory, uint32_t *page_table,
                           void *physical_addr, void *virtual_addr,
                           uint32_t size, PermissionType permission)
{
    uint32_t *page_table_entry = page_table + ((uint32_t)virtual_addr / PAGE_SIZE);
    MapMemoryRangeOffset(page_directory, page_table_entry,
                         physical_addr, virtual_addr,
                         size, permission);
}

void Pager::MapMemoryRangeOffset(uint32_t *page_directory, uint32_t *page_table,
                                 void *physical_addr, void *virtual_addr,
                                 uint32_t size, PermissionType permission)
{
    if ((uint32_t)physical_addr & 0xFFF || size & 0xFFF)
    {
        Screen::Print("Page mappings must be 4KB aligned! (paging.cpp:line 31)");
    }

    void *current_address = physical_addr;
    uint32_t page_index = (uint32_t)virtual_addr / PAGE_SIZE;
    uint32_t page_table_offset = 0;

    page_directory[page_index / 1024] = (uint32_t)(page_table + page_table_offset) | permission | 0x1;

    while ((uint32_t)current_address < (uint32_t)physical_addr + size)
    {
        if (!(page_index % 1024))
        {
            page_directory[page_index / 1024] = (uint32_t)(page_table + page_table_offset) | permission | 0x1;
        }

        page_table[page_table_offset] = ((uint32_t)current_address & 0xFFFFF000) | permission | 0x1;

        current_address = (void *)((uint32_t)current_address + PAGE_SIZE);
        page_index++;
        page_table_offset++;
    }
}

void Pager::UnmapMemoryRange(uint32_t *page_directory, void *virtual_addr, size_t len)
{
    void *current_address = virtual_addr;

    while (current_address < virtual_addr + len)
    {
        *GetPageTableEntry(page_directory, current_address) = 0;
    }
}