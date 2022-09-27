#include <iostream>
#include <stdint.h>

#include "serial_port_util.hpp"

namespace serial {

    class SerialPort
    {
    public:
        SerialPort(const std::string& device);
        SerialPort(const std::string& device, const Config& config);
        ~SerialPort();

        int IsDataReady();
        void SendData(uint8_t* buffer, uint8_t length);
        void ReadData(uint8_t* buffer, uint8_t length);
        std::string GetDeviceName() { return m_Device; }
    private:
        int m_FileDescriptor;
        const std::string m_Device;
        Config m_Config;
    private:
        void OpenDevice();
        void Configure();
    };

}
