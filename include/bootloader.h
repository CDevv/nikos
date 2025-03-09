#ifndef NIKOS_BOOTLOADER_H_

#define NIKOS_BOOTLOADER_H_

#include <limine.h>

struct limine_file* get_module_file(const char* name);

#endif // NIKOS_BOOTLOADER_H_