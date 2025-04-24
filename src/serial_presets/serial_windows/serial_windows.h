#ifndef SERIAL_WINDOWS_H
#define SERIAL_WINDOWS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

bool setContextWindows(char * port, int baud);
bool openPortWindows();
bool closePortWindows();
bool configurePortWindows();
int writeWindows(const char * data, const uint16_t length);
int readWindows(char * bytes, const uint16_t length);
int peekWindows(void);
int getBaudRate(int baudRate);

#ifdef __cplusplus
}
#endif

#endif