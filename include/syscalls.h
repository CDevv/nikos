#ifndef NIKOS_SYSCALLS_H

#define NIKOS_SYSCALLS_H

#include <common/types.h>
#include <hardware/interrupts.h>

using namespace nikos;
using namespace nikos::common;
using namespace nikos::hardware;

namespace nikos
{
    class SyscallHandler : public InterruptHandler
    {
        public:
            SyscallHandler(InterruptManager* interruptManager, uint8_t interruptNumber);
            ~SyscallHandler();
            virtual uint32_t HandleInterrupt(uint32_t esp);
    };
} // namespace nikos


#endif