#ifndef NIKOS_HARDWARE_KEYBOARD_H

#define NIKOS_HARDWARE_KEYBOARD_H

#include <common/types.h>
#include <hardware/port.h>
#include <hardware/interrupts.h>

namespace nikos
{
    namespace hardware
    {
        struct Key
        {
            char normalKey;
            char shiftKey;
            bool empty;
        };

        class Keyboard : public InterruptHandler
        {
            Port8 dataPort;
            Port8 commandPort;

            Key keys[256];

            public:
                Keyboard(InterruptManager* interruptManager);
                ~Keyboard();
                void SetupKeys();
                void AddKey(uint8_t index, char normalKey, char shiftKey);
                void SetEmpty(uint8_t index);
                uint32_t HandleInterrupt(uint32_t esp);
        };
    } // namespace hardware
} // namespace nikos


#endif