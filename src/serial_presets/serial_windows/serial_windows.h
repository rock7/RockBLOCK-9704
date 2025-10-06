#ifndef SERIAL_WINDOWS_H
#define SERIAL_WINDOWS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Sets the serial communication context for a Windows system.
 *
 * @param port The name of the serial port (e.g., "COM1").
 * @param baud The baud rate for communication.
 * @return true if the context was set successfully, false otherwise.
 */
bool setContextWindows(const char * port, const uint32_t baud);

/**
 * @brief Opens the previously configured serial port.
 *
 * @return true if the port was opened successfully, false otherwise.
 */
bool openPortWindows(void);

/**
 * @brief Closes the currently open serial port.
 *
 * @return true if the port was closed successfully, false otherwise.
 */
bool closePortWindows(void);

/**
 * @brief Configures the open serial port with the desired settings.
 *
 * @return true if configuration was successful, false otherwise.
 */
bool configurePortWindows(void);

/**
 * @brief Writes data to the serial port.
 *
 * @param data Pointer to the data to send.
 * @param length Number of bytes to write.
 * @return Number of bytes actually written, or -1 on failure.
 */
int writeWindows(const char * data, const uint16_t length);

/**
 * @brief Reads data from the serial port.
 *
 * @param bytes Buffer to store the received data.
 * @param length Maximum number of bytes to read.
 * @return Number of bytes actually read, or -1 on failure.
 */
int readWindows(char * bytes, const uint16_t length);

/**
 * @brief Peeks at the number of bytes available in the receive buffer.
 *
 * @return Number of bytes available to read, or -1 on error.
 */
int peekWindows(void);

/**
 * @brief Converts a given baud rate enumeration or value to the corresponding system value.
 *
 * @param baudRate The input baud rate identifier or value.
 * @return The system-specific baud rate value, or -1 if unsupported.
 */
int getBaudRate(int baudRate);

#ifdef __cplusplus
}
#endif

#endif // SERIAL_WINDOWS_H
