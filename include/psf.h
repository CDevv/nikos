#ifndef NIKOS_PSF_H_

#define NIKOS_PSF_H_

struct PSF1_HEADER
{
    unsigned char magic[2];
    unsigned char mode;
    unsigned char charsize;
};

struct PSF1_FONT
{
    struct PSF1_HEADER *psf1_header;
    void *glyph_buffer;
};

extern const int glyph_width;
extern const int glyph_height;
struct PSF1_FONT get_font(const char* filename);

#endif // NIKOS_PSF_H_