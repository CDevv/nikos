#ifndef NIKOS_HARDWARE_PORT_H

#define NIKOS_HARDWARE_PORT_H

#include <common/types.h>

using namespace nikos::common;

namespace nikos
{
    namespace hardware
    {
        class Port
        {
            protected:
                uint16_t portNumber;
                Port(uint16_t portNumber);
                ~Port();
        };

        class Port8 : public Port
        {
            public:
                Port8(uint16_t portNumber);
                ~Port8();
                virtual uint8_t Read();
                virtual void Write(uint8_t data);

            protected:
                static inline uint8_t Read8(uint16_t _port)
                {
                    uint8_t result;
                    __asm__ volatile("inb %1, %0" : "=a" (result) : "Nd" (_port));
                    return result;
                }

                static inline void Write8(uint16_t _port, uint8_t _data)
                {
                    __asm__ volatile("outb %0, %1" : : "a" (_data), "Nd" (_port));
                }
        };

        class Port8Slow : public Port8
        {
            public:
                Port8Slow(uint16_t portNumber);
                ~Port8Slow();
                virtual void Write(uint8_t data);

            protected:
                static inline void Write8Slow(uint16_t _port, uint8_t _data)
                {
                    __asm__ volatile("outb %0, %1\njmp 1f\n1: jmp 1f\n1:" : : "a" (_data), "Nd" (_port));
                }
        };

        class Port16 : public Port
        {
            public:
                Port16(uint16_t portNumber);
                ~Port16();
                virtual uint16_t Read();
                virtual void Write(uint16_t data);

            protected:
                static inline uint16_t Read16(uint16_t _port)
                {
                    uint16_t result;
                    __asm__ volatile("inw %1, %0" : "=a" (result) : "Nd" (_port));
                    return result;
                }

                static inline void Write16(uint16_t _port, uint16_t _data)
                {
                    __asm__ volatile("outw %0, %1" : : "a" (_data), "Nd" (_port));
                }
        };

        class Port32 : public Port
        {
            public:
                Port32(uint16_t portNumber);
                ~Port32();
                virtual uint32_t Read();
                virtual void Write(uint32_t data);

            protected:
                static inline uint16_t Read32(uint16_t _port)
                {
                    uint32_t result;
                    __asm__ volatile("inl %1, %0" : "=a" (result) : "Nd" (_port));
                    return result;
                }

                static inline void Write32(uint16_t _port, uint32_t _data)
                {
                    __asm__ volatile("outl %0, %1" : : "a" (_data), "Nd" (_port));
                }
        };

    } // namespace hardware   
} // namespace nikos


#endif