#ifndef SERIAL_LINUX_H
#define SERIAL_LINUX_H

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

//internal functions
bool setContextLinux(char * port, int baud);
bool openPortLinux();
bool closePortLinux();
bool configurePortLinux();
int writeLinux(const char * data, const uint16_t length);
int readLinux(char * bytes, const uint16_t length);
int peekLinux(void);
int getBaudRate(int baudRate);

#ifdef __cplusplus
}
#endif

#endif