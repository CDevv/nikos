#include <bootloader.h>
#include <stdbool.h>
#include <stddef.h>

__attribute__((used, section(".requests")))
static volatile struct limine_module_request module_request = {
    .id = LIMINE_MODULE_REQUEST,
    .revision = 0
};

__attribute__((used, section(".requests_start_marker")))
static volatile LIMINE_REQUESTS_START_MARKER;

__attribute__((used, section(".requests_end_marker")))
static volatile LIMINE_REQUESTS_END_MARKER;

bool checkStringEndsWith(const char* str, const char* end)
{
    const char* _str = str;
    const char* _end = end;

    while(*str != 0)
        str++;
    str--;

    while(*end != 0)
        end++;
    end--;

    while (true)
    {
        if (*str != *end)
            return false;

        str--;
        end--;

        if (end == _end || (str == _str && end == _end))
            return true;

        if (str == _str)
            return false;
    }

    return true;
}

struct limine_file* get_module_file(const char* name)
{
    struct limine_module_response *module_response = module_request.response;

    for (size_t i = 0; i < module_response->module_count; i++) 
    {
        struct limine_file *f = module_response->modules[i];
        if (checkStringEndsWith(f->path, name))
            return f;
    }
    
    return NULL;
}