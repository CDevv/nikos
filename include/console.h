#ifndef NIKOS_CONSOLE_H_

#define NIKOS_CONSOLE_H_

#include <psf.h>

void initiateConsole();
void clear();
void print(const char* str);
void putchar(char c, unsigned int xOff, unsigned int yOff);

static int cursor_x;
static int cursor_y;
static struct PSF1_FONT *font;

#endif