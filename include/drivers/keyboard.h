#ifndef NIKOS_HARDWARE_KEYBOARD_H

#define NIKOS_HARDWARE_KEYBOARD_H

#include <common/types.h>
#include <hardware/port.h>
#include <hardware/interrupts.h>
#include <drivers/driver.h>

using namespace nikos::hardware;

namespace nikos
{
    namespace drivers
    {
        struct Key
        {
            char normalKey;
            char shiftKey;
            bool empty;
        };

        class KeyboardEventHandler
        {
            public:
                KeyboardEventHandler();
                virtual void OnKeyUp(char c);
                virtual void OnKeyDown(char c);
        };

        class Keyboard : public InterruptHandler, public Driver
        {
            Port8 dataPort;
            Port8 commandPort;

            Key keys[256];

            KeyboardEventHandler* eventHandler;

            public:
                Keyboard(InterruptManager* interruptManager, KeyboardEventHandler* eventHandler);
                ~Keyboard();
                void SetupKeys();
                void AddKey(uint8_t index, char normalKey, char shiftKey);
                void SetEmpty(uint8_t index);

                virtual uint32_t HandleInterrupt(uint32_t esp);
                virtual void Activate();
        };
    } // namespace hardware
} // namespace nikos


#endif