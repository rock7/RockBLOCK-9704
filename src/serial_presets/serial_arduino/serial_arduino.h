#ifndef SERIAL_ARDUINO_H
#define SERIAL_ARDUINO_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Opens the Arduino serial port.
 *
 * @return true if the port was opened successfully, false otherwise.
 */
bool openPortArduino(void);

/**
 * @brief Closes the currently open Arduino serial port.
 *
 * @return true if the port was closed successfully, false otherwise.
 */
bool closePortArduino(void);

/**
 * @brief Reads data from the Arduino serial interface.
 *
 * @param bytes Buffer to store the received data.
 * @param length Maximum number of bytes to read.
 * @return Number of bytes actually read.
 */
int readArduino(char * bytes, const uint16_t length);

/**
 * @brief Writes data to the Arduino serial interface.
 *
 * @param data Pointer to the data to send.
 * @param length Number of bytes to write.
 * @return Number of bytes actually written.
 */
int writeArduino(const char * data, const uint16_t length);

/**
 * @brief Sets the communication context for Arduino serial connection.
 *
 * @param port String of the Arduino serial port i.e. Serial1 Serial2
 * @param baud The baud rate for communication (e.g., 9600, 115200), should be 230400 for JSPR.
 * @return true if the context was set successfully, false otherwise.
 */
bool setContextArduino(const char * port, const uint32_t baud);

#ifdef __cplusplus
}
#endif

#endif // SERIAL_ARDUINO_H
