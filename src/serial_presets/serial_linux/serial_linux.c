#if defined(__linux__) || defined(__APPLE__)
#include "serial_linux.h"
#include "serial.h"
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/select.h>
#include <sys/ioctl.h>

//Serial Variables
extern int serialConnection;
extern enum serialState serialState;
extern char* serialPort;
extern int serialBaud;
extern serialContext context;

bool setContextLinux(char* port, int baud)
{
    bool set = false;
    serialPort = port;
    serialBaud = baud;
    context.serialInit = openPortLinux;
    context.serialDeInit = closePortLinux;
    context.serialRead = readLinux;
    context.serialWrite = writeLinux;
    context.serialPeek = peekLinux;

    if(context.serialInit()) //Open and close the port to test
    {
        if(context.serialDeInit())
        {
            set = true;
        }
    }
    return set;
}

bool openPortLinux()
{
    if(serialState != OPEN)
    {
        serialConnection = open(serialPort, O_RDWR | O_NOCTTY | O_SYNC | O_NONBLOCK);
        if(0 > serialConnection)
        {
            return false;
        }
        if(!configurePortLinux())
        {
            return false;
        }
        serialState = OPEN;
        return true;
    }
    else
    {
        fprintf(stderr, "Error: port already open\r\n");
        return false;
    }
}

bool closePortLinux()
{
    if(serialState != CLOSED)
    {
        close(serialConnection);
        serialState = CLOSED;
        return true;
    }
    else
    {
        fprintf(stderr, "Error: port already closed\r\n");
        return false;
    }
}

bool configurePortLinux()
{
struct termios options;

    if(tcgetattr(serialConnection, &options) != 0) 
    {
        fprintf(stderr, "Error: Could not get port attributes\r\n");
        close(serialConnection);
        return false;
    }
    else
    {
        cfsetispeed(&options, getBaudRate(serialBaud));
        cfsetospeed(&options, getBaudRate(serialBaud));

        options.c_cflag &= ~CSIZE;          // Clear the character size mask
        options.c_cflag |= CS8;             // Set 8 data bits
        options.c_cflag &= ~PARENB;         // Disable parity
        options.c_cflag &= ~CSTOPB;         // Use 1 stop bit
        options.c_cflag |= CLOCAL | CREAD; // Enable receiver, local connection
        options.c_iflag &= ~(IXON | IXOFF | IXANY | ICRNL);

        // Disable canonical mode (input is not processed line-by-line)
        options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

        // Set the serial port options
        if (tcsetattr(serialConnection, TCSANOW, &options) != 0) 
        {
            fprintf(stderr, "Error: Could not set port attributes\r\n");
            close(serialConnection);
            return false;
        }
    }
    return true;
}

int getBaudRate(int baudRate)
{
    switch (baudRate) 
    {
        case 0: return B0;
        case 50: return B50;
        case 75: return B75;
        case 110: return B110;
        case 134: return B134;
        case 150: return B150;
        case 200: return B200;
        case 300: return B300;
        case 600: return B600;
        case 1200: return B1200;
        case 1800: return B1800;
        case 2400: return B2400;
        case 4800: return B4800;
        case 9600: return B9600;
        case 19200: return B19200;
        case 38400: return B38400;
        case 57600: return B57600;
        case 115200: return B115200;
        case 230400: return B230400;
#ifdef B460800
        case 460800: return B460800;
#endif
#ifdef B921600
        case 921600: return B921600;
#endif
        default:
            fprintf(stderr, "Unsupported baud rate: %d\r\n", baudRate);
            return B230400;
    }
}

int readLinux(char *bytes, const uint16_t length)
{
    if (serialState == OPEN)
    {
        int bytesRead = 0;
        char ch;

        struct timeval timeout = {0, 500000};
        fd_set read_fds;
        FD_ZERO(&read_fds);
        FD_SET(serialConnection, &read_fds);
        int ready = select(serialConnection + 1, &read_fds, NULL, NULL, &timeout);
        if (ready < 0)
        {
            fprintf(stderr, "Error: Failed while waiting for data\r\n");
            return -1;
        }
        if (ready == 0) // Timeout, no data available
        {
            return -1;
        }
        while (bytesRead < length)
        {
            int result = read(serialConnection, &ch, 1);
            if (result < 0)
            {
                fprintf(stderr, "Error: Could not read from serial port\r\n");
                return -1;
            }
            bytes[bytesRead] = ch;
            bytesRead++;
        }
        bytes[bytesRead] = '\0';
        return bytesRead;
    }
    else
    {
        fprintf(stderr, "Error: port not open, can't read\r\n");
        return -1;
    }
}

int writeLinux(const char * data, const uint16_t length)
{
    int rc = -1;
    bool retry = false;

    if(serialState == OPEN)
    {
        do
        {
            retry = false;
            rc = write(serialConnection, data, length);
            if (rc < 0)
            {
                retry = (errno == EAGAIN) ? true : false;
            }
        } while (retry == true);

        if (rc < 0)
        {
            fprintf(stderr, "Error: Could not write to serial port %s\r\n", (char*)strerror(errno));
        }
    }
    else
    {
        fprintf(stderr, "Error: port not open, can't write\r\n");
    }

    return rc;
}

int peekLinux(void)
{
    int bytes = 0;
    if (serialConnection > 0)
    {
        if (ioctl(serialConnection, FIONREAD, &bytes) != 0)
        {
            bytes = -1;
        }
    }
    return bytes;
}
#endif