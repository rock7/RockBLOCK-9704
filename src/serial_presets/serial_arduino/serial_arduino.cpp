#ifdef ARDUINO
#include "serial_arduino.h"
#include "../../serial.h"

//Serial Variables
extern int serialConnection;
extern enum serialState serialState;
Stream &serialPortArduino = Serial1;
extern serialContext context;

bool openPortArduino()
{
    serialState = OPEN;
    serialPortArduino.setTimeout(1000);
    return true;
}

bool closePortArduino()
{
    serialState = CLOSED;
    return true;
}

int readArduino(char * bytes, const uint16_t length)
{
    return (int)serialPortArduino.readBytes(bytes, length);
}

int writeArduino(const char * data, const uint16_t length)
{
    return (int)serialPortArduino.write(data, length);
}

int peekArduino(void)
{
    return (int)serialPortArduino.available();
}

bool setContextArduino(Stream &port, const uint32_t baud)
{
bool set = false;

    serialPortArduino = port;
    context.serialBaud = baud;
    context.serialInit = openPortArduino;
    context.serialDeInit = closePortArduino;
    context.serialRead = readArduino;
    context.serialWrite = writeArduino;
    context.serialPeek = peekArduino;

    if(context.serialInit()) //Open and close the port to test
    {
        if(context.serialDeInit())
        {
            set = true;
        }
    }
    return set;
}
#endif