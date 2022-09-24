#include <exception>
#include <string>

#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>

namespace serial {

    enum class Parity
    {
        NONE, ODD, EVEN
    };

    enum class StopBits
    {
        ONE, TWO
    };

    enum class DataBits
    {
        SIX = CS6,
        SEVEN = CS7,
        EIGHT = CS8
    };

    enum class Baudrate
    {
        BAUD9600 = B9600,
        BAUD19200 = B19200,
        BAUD115200 = B115200
    };

    struct Config
    {
        Parity parity;
        StopBits stop_bits;
        DataBits data_bits;
        Baudrate baudrate;
    };

    class SerialPortException : public std::exception
    {
    public:
        SerialPortException(const std::string& message, const std::string errorDescription)
            : m_Message(message), m_ErrorDescription(errorDescription)
        {
        }

        const char* what() const throw() override
        {
            static std::string msg = "SerialPortException {\n\tmessage: " + m_Message + "\n\terror: " + m_ErrorDescription + "\n}";
            return msg.c_str();
        }
    private:
        const std::string m_Message;
        const std::string m_ErrorDescription;
    };

}
