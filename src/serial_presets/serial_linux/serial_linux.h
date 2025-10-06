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

/**
 * @brief Sets the serial communication context for a Linux system.
 *
 * @param port The name of the serial port (e.g., "/dev/ttyUSB0").
 * @param baud The baud rate for communication.
 * @return true if the context was set successfully, false otherwise.
 */
bool setContextLinux(const char * port, const uint32_t baud);

/**
 * @brief Opens the previously configured serial port.
 *
 * @return true if the port was opened successfully, false otherwise.
 */
bool openPortLinux(void);

/**
 * @brief Closes the currently open serial port.
 *
 * @return true if the port was closed successfully, false otherwise.
 */
bool closePortLinux(void);

/**
 * @brief Configures the open serial port with the desired settings.
 *
 * @return true if configuration was successful, false otherwise.
 */
bool configurePortLinux(void);

/**
 * @brief Writes data to the serial port.
 *
 * @param data Pointer to the data to send.
 * @param length Number of bytes to write.
 * @return Number of bytes actually written, or -1 on failure.
 */
int writeLinux(const char * data, const uint16_t length);

/**
 * @brief Reads data from the serial port.
 *
 * @param bytes Buffer to store the received data.
 * @param length Maximum number of bytes to read.
 * @return Number of bytes actually read, or -1 on failure.
 */
int readLinux(char * bytes, const uint16_t length);

/**
 * @brief Peeks at the number of bytes available in the receive buffer.
 *
 * @return Number of bytes available to read, or -1 on error.
 */
int peekLinux(void);

/**
 * @brief Maps a standard baud rate to the corresponding Linux system constant.
 *
 * @param baud The desired baud rate (e.g., 9600, 115200).
 * @return The system-specific baud rate constant, or 230400 by default.
 */
uint32_t getBaudRate(const uint32_t baud);

#ifdef __cplusplus
}
#endif

#endif // SERIAL_LINUX_H
