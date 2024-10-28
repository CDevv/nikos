#include <hardware/port.h>

using namespace nikos;
using namespace nikos::hardware;

Port::Port(uint16_t portNumber)
{
    this->portNumber = portNumber;
}

Port::~Port() {}

//8bit ports
Port8::Port8(uint16_t portNumber)
    : Port(portNumber)
{
}

Port8::~Port8() {}

uint8_t Port8::Read()
{
    return Read8(portNumber);
}

void Port8::Write(uint8_t data)
{
    Write8(portNumber, data);
}

//slow 8bit ports
Port8Slow::Port8Slow(uint16_t portNumber)
    : Port8(portNumber)
{
}

Port8Slow::~Port8Slow() {}

void Port8Slow::Write(uint8_t data)
{
    Write8Slow(portNumber, data);
}

//16bit ports
Port16::Port16(uint16_t portNumber)
    : Port(portNumber)
{
}

Port16::~Port16() {}

uint16_t Port16::Read()
{
    return Read16(portNumber);
}

void Port16::Write(uint16_t data)
{
    Write16(portNumber, data);
}

//32bit ports
Port32::Port32(uint16_t portNumber)
    : Port(portNumber)
{
}

Port32::~Port32() {}

uint32_t Port32::Read()
{
    return Read32(portNumber);
}

void Port32::Write(uint32_t data)
{
    Write32(portNumber, data);
}