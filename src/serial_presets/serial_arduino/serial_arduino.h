#ifndef SERIAL_ARDUINO_H
#define SERIAL_ARDUINO_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

bool openPortArduino(void);
bool closePortArduino(void);
int readArduino(char * bytes, const uint16_t length);
int writeArduino(const char * data, const uint16_t length);
bool setContextArduino(char * port, int baud);

#ifdef __cplusplus
}
#endif

#endif