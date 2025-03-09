#ifndef NIKOS_VGA_H_

#define NIKOS_VGA_H_

#include <limine.h>
#include <stddef.h>

struct Framebuffer
{
    void *base_address;
    size_t buffer_size;
    unsigned int width;
    unsigned int height;
    unsigned int pixels_per_scan_line;
};

void initiateVGA();

extern struct Framebuffer *framebuffer;

#endif // NIKOS_VGA_H_