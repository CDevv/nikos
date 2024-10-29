#ifndef NIKOS_DRIVERS_KEYBOARDHANDLERS_H

#define NIKOS_DRIVERS_KEYBOARDHANDLERS_H

#include <drivers/keyboard.h>

namespace nikos
{
    namespace drivers
    {
        class CLIKeyboard : public KeyboardEventHandler
        {
            public:
                void OnKeyDown(char c);
        };
    } // namespace drivers  
} // namespace nikos


#endif