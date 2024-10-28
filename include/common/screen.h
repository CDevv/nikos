#ifndef NIKOS_COMMON_SCREEN_H

#define NIKOS_COMMON_SCREEN_H

#include <common/types.h>

namespace nikos
{
    namespace common
    {
        enum VGAColor
        {
            VGA_COLOR_BLACK = 0,
            VGA_COLOR_BLUE = 1,
            VGA_COLOR_GREEN = 2,
            VGA_COLOR_CYAN = 3,
            VGA_COLOR_RED = 4,
            VGA_COLOR_MAGENTA = 5,
            VGA_COLOR_BROWN = 6,
            VGA_COLOR_LIGHT_GREY = 7,
            VGA_COLOR_DARK_GREY = 8,
            VGA_COLOR_LIGHT_BLUE = 9,
            VGA_COLOR_LIGHT_GREEN = 10,
            VGA_COLOR_LIGHT_CYAN = 11,
            VGA_COLOR_LIGHT_RED = 12,
            VGA_COLOR_LIGHT_MAGENTA = 13,
            VGA_COLOR_LIGHT_BROWN = 14,
            VGA_COLOR_WHITE = 15,
        };

        class Screen
        {
            private:
                static int x;
                static int y;
                static uint8_t MakeColor(VGAColor foreground, VGAColor background);
                static uint16_t VGAEntry(uint8_t c, uint8_t color);
            public:
                static void PutChar(char c, VGAColor foreground, VGAColor background);
                static void Print(char* str, VGAColor foreground = VGA_COLOR_WHITE, VGAColor background = VGA_COLOR_BLACK);
                static void PrintHex(uint8_t num);
        };
    } // namespace common
    
} // namespace nikos


#endif