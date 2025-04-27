#include <vga.h>
#include <stddef.h>
#include <limine.h>
#include <bootloader.h>

__attribute__((used, section(".requests"))) 
static volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0};

struct Framebuffer fb;
struct Framebuffer *framebuffer;

void initiateVGA()
{
    if (framebuffer_request.response == NULL || framebuffer_request.response->framebuffer_count < 1)
    {
        hcf();
    }

    struct limine_framebuffer *limine_framebuffer = framebuffer_request.response->framebuffers[0];

    fb.base_address = limine_framebuffer->address;
    fb.width = limine_framebuffer->width;
    fb.height = limine_framebuffer->height;
    fb.pixels_per_scan_line = limine_framebuffer->pitch / 4;
    fb.buffer_size = limine_framebuffer->height * limine_framebuffer->pitch;

    framebuffer = &fb;
}

void putPixel(int x, int y)
{
    unsigned int *pixPtr = (unsigned int *)framebuffer->base_address;
    
    *(unsigned int *)(pixPtr + x + (y * framebuffer->pixels_per_scan_line)) = 0xffffffff;
}