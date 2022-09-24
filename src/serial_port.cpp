#include "serial_port.hpp"
#include <string.h>

namespace serial {

    SerialPort::SerialPort(const std::string& device)
        : SerialPort(device, {Parity::NONE, StopBits::ONE, DataBits::EIGHT, Baudrate::BAUD115200})
    {
    }

    SerialPort::SerialPort(const std::string& device, const Config& config)
        : m_Device(device), m_Config(config)
    {
        OpenDevice();
        Configure();
    }

    SerialPort::~SerialPort()
    {
        close(m_FileDescriptor);
    }

    int SerialPort::IsDataReady()
    {
        int bytes_ready = 0;
        ioctl(m_FileDescriptor, FIONREAD, &bytes_ready);
        return bytes_ready;
    }

    void SerialPort::SendData(uint8_t* buffer, uint8_t length)
    {
        int bytes_written = write(m_FileDescriptor, buffer, length);
        if (bytes_written < 0) {
            throw SerialPortException("Error sending data.", strerror(errno));
        } else if (bytes_written < length) {
            throw SerialPortException("Less bytes written that expected.", strerror(errno));
        }
    }

    void SerialPort::ReadData(uint8_t* buffer, uint8_t length)
    {
        int bytes_read = read(m_FileDescriptor, buffer, length);
        if (bytes_read < 0) {
            throw SerialPortException("Error reading data.", strerror(errno));
        }
    }

    void SerialPort::OpenDevice()
    {
        m_FileDescriptor = open(m_Device.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
        if (m_FileDescriptor < 0) {
            throw SerialPortException("System call open() couldn't open file.", strerror(errno));
        }

        fcntl(m_FileDescriptor, F_SETFL, 0);
    }

    void SerialPort::Configure()
    {
        termios tty;

        if (!isatty(m_FileDescriptor)) {
            throw SerialPortException("Opened file isn't a tty device.", strerror(errno));
        }

        if (tcgetattr(m_FileDescriptor, &tty) < 0) {
            throw SerialPortException("Couldn't get serial interface configuration.", strerror(errno));
        }

        // configure parity
        switch (m_Config.parity)
        {
        case Parity::NONE:
            tty.c_cflag &= ~PARENB;
            tty.c_iflag &= ~(INPCK | PARMRK | ISTRIP);
            break;
        case Parity::ODD:
            tty.c_cflag |= (PARENB | PARODD);
            tty.c_iflag |= (INPCK | PARMRK | ISTRIP);
            break;
        case Parity::EVEN:
            tty.c_cflag |= PARENB;
            tty.c_cflag &= ~PARODD;
            tty.c_iflag |= (INPCK | PARMRK | ISTRIP);
            break;
        }

        // configure stop bits
        switch (m_Config.stop_bits)
        {
        case StopBits::ONE:
            tty.c_cflag &= ~CSTOPB;
            break;
        case StopBits::TWO:
            tty.c_cflag |= CSTOPB;
            break;
        }

        // configure baudrate
        if (cfsetspeed(&tty, static_cast<unsigned int>(m_Config.baudrate)) < 0) {
            throw SerialPortException("Failed to set baud rate.", strerror(errno));
        }

        // configure data bits
        tty.c_cflag &= ~CSIZE;
        tty.c_cflag |= static_cast<unsigned int>(m_Config.data_bits);

        // Disable RTS/CTS hardware flow control
        tty.c_cflag &= ~CRTSCTS;
        // Turn on READ & ignore ctrl lines (CLOCAL = 1)
        tty.c_cflag |= (CREAD|CLOCAL);

        // Local modes
        tty.c_lflag &= ~ICANON;         // Disable canonical mode
        tty.c_lflag &= ~ECHO;           // Disable echo
        tty.c_lflag &= ~ECHOE;          // Disable erasure
        tty.c_lflag &= ~ECHONL;         // Disable new-line echo
        tty.c_lflag &= ~ISIG;           // Disable interpretation of INTR, QUIT and SUSP

        // Input modes
        tty.c_iflag &= ~(IXON|IXOFF|IXANY);                                 // Turn off software flow control
        tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL);    // Disable any special handling of received bytes

        // Output modes
        tty.c_oflag &= ~OPOST;  // Prevent special interpretation of output bytes (e.g. newline chars)
        tty.c_oflag &= ~ONLCR;  // Prevent conversion of newline to carriage return/line feed

        // Control characters
        tty.c_cc[VTIME] = 0;
        tty.c_cc[VMIN] = 1;

        // Flush the input buffer
        if (tcflush(m_FileDescriptor, TCIFLUSH) < 0) {
            throw SerialPortException("Failed to flush input buffer.", strerror(errno));
        }

        // Save tty settings, also checking for error
        if (tcsetattr(m_FileDescriptor, TCSANOW, &tty) != 0 ) {
            throw SerialPortException("Failed to save tty configuration.", strerror(errno));
        }
    }

}
