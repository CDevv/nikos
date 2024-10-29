#include <drivers/keyboardHandlers.h>
#include <common/screen.h>

using namespace nikos::drivers;

void CLIKeyboard::OnKeyDown(char c)
{
    Screen::PutChar(c, VGA_COLOR_WHITE, VGA_COLOR_BLACK);
}