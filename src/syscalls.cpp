#include <syscalls.h>
#include <common/screen.h>

using namespace nikos;

SyscallHandler::SyscallHandler(InterruptManager* interruptManager, uint8_t interruptNumber)
    : InterruptHandler(interruptNumber + interruptManager->HardwareInterruptOffset(), interruptManager)
{
}

SyscallHandler::~SyscallHandler() {}

uint32_t SyscallHandler::HandleInterrupt(uint32_t esp)
{
    CPUState* cpuState = (CPUState*)esp;

    switch (cpuState->eax)
    {
        case 4:
            Screen::Print((char*)cpuState->ebx);
            break;
        
        default:
            break;
    }

    return esp;
}