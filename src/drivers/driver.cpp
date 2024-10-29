#include <drivers/driver.h>

using namespace nikos;
using namespace nikos::drivers;

Driver::Driver()
{
}

Driver::~Driver()
{
}

void Driver::Activate()
{
}

void Driver::Deactivate()
{
}

int Driver::Reset()
{
    return 0;
}

DriverManager::DriverManager()
{
    numDrivers = 0;
}

DriverManager::~DriverManager() {}

void DriverManager::ActivateAll()
{
    for (int i = 0; i < numDrivers; i++)
    {
        this->drivers[i]->Activate();
    }
}

void DriverManager::AddDriver(Driver* driver)
{
    if (numDrivers <= 256)
    {
        this->drivers[numDrivers] = driver;
        numDrivers++;
    }
}