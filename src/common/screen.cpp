#include <common/screen.h>

using namespace nikos;
using namespace nikos::common;

const int maxX = 80;
const int maxY = 25;

int Screen::x = 0;
int Screen::y = 0;

uint8_t Screen::MakeColor(VGAColor foreground, VGAColor background)
{
    return foreground | background << 4;
}

uint16_t Screen::VGAEntry(uint8_t c, uint8_t color)
{
    return (uint16_t)c | (uint16_t)color << 8;
}

void Screen::PutChar(char c, VGAColor foreground, VGAColor background)
{
    uint16_t* VideoMemory = (uint16_t*)0xb8000;

    if (c == '\n')
    {
        x = 0;
        y++;
        return;
    }   

    if (x >= maxX)
    {
        x = 0;
        y++;
    }
    
    if (y >= maxY)
    {
        y = 0;
    }

    VideoMemory[80*y+x] = VGAEntry(c, MakeColor(foreground, background));
    x++;
}

void Screen::Print(char* str, VGAColor foreground, VGAColor background)
{
    for(int i = 0; str[i] != '\0'; ++i)
    {
        PutChar(str[i], foreground, background);
    }
}

void Screen::PrintHex(uint8_t num)
{
    char* foo = "00";
    char* hex = "0123456789ABCDEF";
    foo[0] = hex[(num >> 4) & 0xF];
    foo[1] = hex[num & 0xF];
    Print(foo);
}

void Screen::PrintInt(uint32_t num)
{
    if (num == 0)
    {
        Screen::Print("0");
        return;
    }

    char buf[32] = "                               ";
    int indexer = 31;

    while (num > 0)
    {
        int remainder = num % 10;
        char c = remainder + '0';
        buf[indexer] = c;
        num = num / 10;
        indexer--;
    }

    for (int i = 0; i < 32; i++)
    {
        char c = buf[i];
        if (c != ' ')
        {
            Screen::PutChar(c, VGA_COLOR_WHITE, VGA_COLOR_BLACK);
        }
    }
}