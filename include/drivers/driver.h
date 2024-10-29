#ifndef NIKOS_DRIVERS_DRIVER_H

#define NIKOS_DRIVERS_DRIVER_H

namespace nikos
{
    namespace drivers
    {
        class Driver
        {
        public:
            Driver();
            ~Driver();

            virtual void Activate();
            virtual void Deactivate();
            virtual int Reset();
        };

        class DriverManager
        {
        private:
            Driver *drivers[256];
            int numDrivers;

        public:
            DriverManager();
            ~DriverManager();

            void AddDriver(Driver *driver);
            void ActivateAll();
        };
    } // namespace drivers
} // namespace nikos

#endif