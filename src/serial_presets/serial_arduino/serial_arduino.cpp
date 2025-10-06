#ifdef ARDUINO
#include "serial_arduino.h"
#include "../../serial.h"
#include <Stream.h>
#include "Arduino.h"

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

bool setContextArduino(const char * port, const uint32_t baud)
{
    strncpy(context.serialPort, port, SERIAL_PORT_LENGTH);
    if(strcmp(context.serialPort, "Serial1") == 0)
    {
        serialPortArduino = Serial1;
    }
#if defined(AVR_MEGA2560) || defined(ARDUINO_SAM_DUE) || defined(ARDUINO_GIGA)
    else if(strcmp(context.serialPort, "Serial2") == 0)
    {
        serialPortArduino = Serial2;
    }
    else if(strcmp(context.serialPort, "Serial3") == 0)
    {
        serialPortArduino = Serial3;
    }
#if defined(ARDUINO_GIGA)
    else if(strcmp(context.serialPort, "Serial4") == 0)
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