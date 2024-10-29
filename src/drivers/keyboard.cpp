#include <drivers/keyboard.h>
#include <common/screen.h>

using namespace nikos;
using namespace nikos::common;
using namespace nikos::hardware;
using namespace nikos::drivers;

KeyboardEventHandler::KeyboardEventHandler() {}

void KeyboardEventHandler::OnKeyUp(char c) {}
void KeyboardEventHandler::OnKeyDown(char c) {}

Keyboard::Keyboard(InterruptManager* interruptManager, KeyboardEventHandler* eventHandler)
    : InterruptHandler(0x21, interruptManager),
    dataPort(0x60),
    commandPort(0x64)
{
    SetupKeys();
    this->eventHandler = eventHandler;
}

void Keyboard::Activate()
{
    while (commandPort.Read() & 0x1)
    {
        dataPort.Read();
    }
    commandPort.Write(0xAE);
    dataPort.Write(0x20);

    uint8_t status = (dataPort.Read() | 1) & ~0x10;
    commandPort.Write(0x60);
    dataPort.Write(status);
    dataPort.Write(0xF4);
}

Keyboard::~Keyboard() {}

uint32_t Keyboard::HandleInterrupt(uint32_t esp)
{
    static bool shiftOn = false;

    uint8_t key = dataPort.Read();

    if (key == 0x2A)
    {
        shiftOn = true;
    }
    else if (key == 0xAA)
    {
        shiftOn = false;
    }
    else if (key < 256)
    {
        if (!keys[key].empty)
        {
            if (shiftOn)
            {
                //Screen::PutChar(keys[key].shiftKey, VGA_COLOR_WHITE, VGA_COLOR_BLACK);
                eventHandler->OnKeyDown(keys[key].shiftKey);
            }
            else
            {
                //Screen::PutChar(keys[key].normalKey, VGA_COLOR_WHITE, VGA_COLOR_BLACK);
                eventHandler->OnKeyDown(keys[key].normalKey);
            }
        }
    }

    return esp;
}

void Keyboard::SetupKeys()
{
    for (int i = 0; i < 256; i++)
    {
        SetEmpty(i);
    }

    AddKey(0x02, '1', '1');
    AddKey(0x03, '2', '2');
    AddKey(0x04, '3', '3');
    AddKey(0x05, '4', '4');
    AddKey(0x06, '5', '5');
    AddKey(0x07, '6', '6');
    AddKey(0x08, '7', '7');
    AddKey(0x09, '8', '8');
    AddKey(0x0A, '9', '9');
    AddKey(0x0B, '0', '0');

    AddKey(0x10, 'q', 'Q');
    AddKey(0x11, 'w', 'W');
    AddKey(0x12, 'e', 'E');
    AddKey(0x13, 'r', 'R');
    AddKey(0x14, 't', 'T');
    AddKey(0x15, 'y', 'Y');
    AddKey(0x16, 'u', 'U');
    AddKey(0x17, 'i', 'I');
    AddKey(0x18, 'o', 'O');
    AddKey(0x19, 'p', 'P');

    AddKey(0x1E, 'a', 'A');
    AddKey(0x1F, 's', 'S');
    AddKey(0x20, 'd', 'D');
    AddKey(0x21, 'f', 'F');
    AddKey(0x22, 'g', 'G');
    AddKey(0x23, 'h', 'H');
    AddKey(0x24, 'j', 'J');
    AddKey(0x25, 'k', 'K');
    AddKey(0x26, 'l', 'L');

    AddKey(0x2C, 'z', 'Z');
    AddKey(0x2D, 'x', 'X');
    AddKey(0x2E, 'c', 'C');
    AddKey(0x2F, 'v', 'V');
    AddKey(0x30, 'b', 'B');
    AddKey(0x31, 'n', 'N');
    AddKey(0x32, 'm', 'M');
    AddKey(0x33, ',', ',');
    AddKey(0x34, '.', '.');
    AddKey(0x35, '-', '-');

    AddKey(0x1C, '\n', '\n');
    AddKey(0x39, ' ', ' ');
}

void Keyboard::AddKey(uint8_t index, char normalKey, char shiftKey)
{
    Key key;
    key.normalKey = normalKey;
    key.shiftKey = shiftKey;
    key.empty = false;

    keys[index] = key;
}

void Keyboard::SetEmpty(uint8_t index)
{
    Key key;
    key.empty = true;

    keys[index] = key;
}