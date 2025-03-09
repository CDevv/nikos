#include <psf.h>
#include <stddef.h>
#include <system.h>
#include <limine.h>
#include <bootloader.h>

const int glyph_height = 16;
const int glyph_width = 8;

struct PSF1_FONT get_font(const char* filename)
{
    struct PSF1_FONT font;
    struct limine_file *font_file = get_module_file(filename);

    if (font_file == NULL)
    {
        hcf();
    }
    
    font.psf1_header = (struct PSF1_HEADER *)font_file->address;

    if (font.psf1_header->magic[0] != 0x36 || font.psf1_header->magic[1] != 0x04)
    {
        hcf();
    }
    
    font.glyph_buffer = (void *)((uint64_t)font_file->address + sizeof(struct PSF1_HEADER));

    return font;
}