#include <console.h>
#include <vga.h>
#include <psf.h>

struct PSF1_FONT psfFont;

void initiateConsole()
{
    cursor_x = 0;
    cursor_y = 0;

    psfFont = get_font("zap-light16.psf");
    font = &psfFont;
}

void print(const char *str)
{
    char *chr = (char *)str;
    while (*chr != 0)
    {
        switch (*chr)
        {
        case '\n':
            cursor_x = 0;
            cursor_y += glyph_height;
            break;
        
        default:
            putchar(*chr, cursor_x, cursor_y);
            cursor_x += glyph_width;
            break;
        }

        if (cursor_x + 8 > framebuffer->width)
        {
            cursor_x = 0;
            cursor_y += glyph_height;
        }

        chr++;
    }
}

void putchar(char c, unsigned int xOff, unsigned int yOff)
{
    unsigned int *pixPtr = (unsigned int *)framebuffer->base_address;
    char *fontPtr = (char *)font->glyph_buffer + (c * font->psf1_header->charsize);

    for (unsigned long y = yOff; y < yOff + glyph_height; y++)
    {
        for (unsigned long x = xOff; x < xOff + glyph_width; x++)
        {
            if ((*fontPtr & (0b10000000 >> (x - xOff))) > 0)
            {
                *(unsigned int *)(pixPtr + x + (y * framebuffer->pixels_per_scan_line)) = 0xffffffff;
            }
        }
        fontPtr++;
    }
}

void clear() {}