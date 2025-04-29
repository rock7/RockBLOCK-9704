#ifdef ARDUINO
#include "serial_arduino.h"
#include "../../serial.h"
#include <Stream.h>
#include "Arduino.h"

//Serial Variables
extern int serialConnection;
extern enum serialState serialState;
Stream &serialPortArduino = Serial1;
extern int serialBaud;
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
    return serialPortArduino.readBytes(bytes, length);
}

int writeArduino(const char * data, const uint16_t length)
{
    return serialPortArduino.write(data, length);
}

bool setContextArduino(char * port, int baud)
{
    if(strcmp(port, "Serial1") == 0)
    {
        serialPortArduino = Serial1;
    }
//JS TODO: double check the preprocessor defines below
#ifdef defined(AVR_MEGA2560) || defined(ARDUINO_SAM_DUE) || defined(ARDUINO_GIGA)
    else if(strcmp(port, "Serial2") == 0)
    {
        serialPortArduino = Serial2;
    }
    else if(strcmp(port, "Serial3") == 0)
    {
        serialPortArduino = Serial3;
    }
#ifdef defined(ARDUINO_GIGA)
    else if(strcmp(port, "Serial4") == 0)
    {
        serialPortArduino = Serial4;
    }
#endif
#endif
    else
    {
        serialPortArduino = Serial1;
    }

    bool set = false;
    serialBaud = baud;
    context.serialInit = openPortArduino;
    context.serialDeInit = closePortArduino;
    context.serialRead = readArduino;
    context.serialWrite = writeArduino;

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