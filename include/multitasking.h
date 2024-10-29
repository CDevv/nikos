#ifndef NIKOS_MULTITASKING_H

#define NIKOS_MULTITASKING_H

#include <common/types.h>
#include <gdt.h>

using namespace nikos;
using namespace nikos::common;

namespace nikos
{
    struct CPUState
    {
        uint32_t eax;
        uint32_t ebx;
        uint32_t ecx;
        uint32_t edx;
        uint32_t esi;
        uint32_t edi;
        uint32_t ebp;

        uint32_t error;

        uint32_t eip;
        uint32_t cs;
        uint32_t eflags;
        uint32_t esp;
        uint32_t ss;
    } __attribute__((packed));

    class Task
    {
        friend class TaskManager;
    private:
        uint8_t stack[4096];
        CPUState* cpuState;

    public:
        Task(GlobalDescriptorTable *gdt, void entrypoint());
        ~Task();
    };

    class TaskManager
    {
    private:
        Task *tasks[256];
        int numTasks;
        int currentTask;

    public:
        TaskManager();
        ~TaskManager();
        bool AddTask(Task *task);
        CPUState* Schedule(CPUState* cpuState);
    };
} // namespace nikos

#endif