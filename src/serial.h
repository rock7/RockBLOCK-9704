#ifndef SERIAL_H
#define SERIAL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#if defined(_WIN32)
#include <io.h>
#define access _access
#else
#include <unistd.h>
#endif

#if defined(__linux__) || defined(__APPLE__)
    #include "serial_presets/serial_linux/serial_linux.h"
#elif defined(_WIN32)
    #include "serial_presets/serial_windows/serial_windows.h"
#elif ARDUINO
    #include "serial_presets/serial_arduino/serial_arduino.h"
#endif

#define SERIAL_PORT_LENGTH 50U // Should be more than enough, don't want to use PATH_MAX as it will be wasteful

// Callback functions which will link to the serial interface
typedef bool(*serialInitFunc)();
typedef bool(*serialDeInitFunc)();
typedef int(*serialReadFunc)(char * bytes, const uint16_t length);
typedef int(*serialWriteFunc)(const char * data, const uint16_t length);
typedef int(*serialPeekFunc)(void);

typedef struct
{
    serialInitFunc           serialInit;
    serialDeInitFunc         serialDeInit;
    serialReadFunc           serialRead;
    serialWriteFunc          serialWrite;
    serialPeekFunc           serialPeek;
    char                     serialPort[SERIAL_PORT_LENGTH];
    uint32_t                 serialBaud;
} serialContext;

enum serialState
{
    CLOSED,
    OPEN,
};

#ifdef __cplusplus
}
#endif

#endif