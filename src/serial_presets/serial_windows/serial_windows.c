#if defined(_WIN32)
#include "serial.h"
#include "serial_windows.h"
#include <windows.h>
#include <stdio.h>
#include <stdbool.h>

// Serial Variables
extern HANDLE serialConnection;
extern enum serialState serialState;
extern char* serialPort;
extern int serialBaud;
extern serialContext context;

bool setContextWindows(char* port, int baud)
{
    bool set = false;
    serialPort = port;
    serialBaud = baud;
    context.serialInit = openPortWindows;
    context.serialDeInit = closePortWindows;
    context.serialRead = readWindows;
    context.serialWrite = writeWindows;
    context.serialPeek = peekWindows;

    if (context.serialInit()) // Open and close the port to test
    {
        if (context.serialDeInit())
        {
            set = true;
        }
    }
    return set;
}

bool openPortWindows()
{
    bool opened = false;
    if (serialState != OPEN)
    {
        serialConnection = CreateFileA(serialPort, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
        if (serialConnection != INVALID_HANDLE_VALUE)
        {
            if (configurePortWindows())
            {
                serialState = OPEN;
                opened = true;
            }
        }
    }
    else
    {
        fprintf(stderr, "Error: port already open\r\n");
    }
    return opened;
}

bool closePortWindows()
{
    bool closed = false;
    if (serialState != CLOSED)
    {
        CloseHandle(serialConnection);
        serialState = CLOSED;
        closed = true;
    }
    return closed;
}

bool configurePortWindows()
{
    bool configured = false;
    DCB dcbSerialParams = {0};
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    if (GetCommState(serialConnection, &dcbSerialParams))
    {
        dcbSerialParams.BaudRate = serialBaud;
        dcbSerialParams.ByteSize = 8;
        dcbSerialParams.Parity = NOPARITY;
        dcbSerialParams.StopBits = ONESTOPBIT;

        if (SetCommState(serialConnection, &dcbSerialParams))
        {
            COMMTIMEOUTS cto;
            if(GetCommTimeouts(serialConnection, &cto))
            {
                cto.ReadIntervalTimeout = 0;
                cto.ReadTotalTimeoutConstant = 500;
                cto.ReadTotalTimeoutMultiplier = 0;
                if(SetCommTimeouts(serialConnection, &cto))
                {
                    configured = true;
                }
            }
        }
        else
        {
            fprintf(stderr, "Error: Could not set port attributes\r\n");
            CloseHandle(serialConnection);
        }
    }
    else
    {
        fprintf(stderr, "Error: Could not get port attributes\r\n");
        CloseHandle(serialConnection);
    }

    return configured;
}

int readWindows(char* bytes, const uint16_t length)
{
    DWORD bytesRead = -1;

    if (serialState == OPEN)
    {
        if (ReadFile(serialConnection, bytes, length, &bytesRead, NULL) != true)
        {
            bytesRead = -1;
        }
    }

    return bytesRead;
}

int writeWindows(const char* data, const uint16_t length)
{
    DWORD bytesWritten = -1;
    if (serialState == OPEN)
    {
        if (WriteFile(serialConnection, data, length, &bytesWritten, NULL) != true)
        {
            bytesWritten = -1;
        }
        return bytesWritten;
    }

    return bytesWritten;
}

int peekWindows()
{
    COMSTAT status;
    DWORD errors;
    if (ClearCommError(serialConnection, &errors, &status))
    {
        return status.cbInQue;
    }
    return -1;
}
#endif
