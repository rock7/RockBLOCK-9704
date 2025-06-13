#ifndef ROCKBLOCK_9704_H
#define ROCKBLOCK_9704_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file rockblock_9704.h
 * @brief The RockBLOCK 9704 library. Maintained by Ground Control (https://www.groundcontrol.com/)
 * and license under the MIT License.
 */

#include "serial.h"
#include "jspr.h"
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stddef.h>

/**
 * @brief Temporary buffer size used for Base64 encoding/decoding of IMT messages.
 */
#define BASE64_TEMP_BUFFER 2048U

/**
 * @brief Fixed serial baud rate for communication with the RockBLOCK 9704 modem.
 *
 * This value should not be changed, as it is required by the modem hardware.
 */
#define RB9704_BAUD 230400U

/**
 * @def SERIAL_CONTEXT_SETUP_FUNC
 * @brief Platform-specific macro to define the serial context setup function.
 *
 * This macro resolves to a platform-appropriate function used to configure the serial context.
 * By default, it maps to:
 * - `setContextLinux` on Linux and macOS
 * - `setContextWindows` on Windows
 * - `setContextArduino` on Arduino platforms
 *
 * You can override this macro by defining `SERIAL_CONTEXT_SETUP_FUNC` manually **before**
 * including this library, allowing for custom serial context initialisation logic.
 *
 * Example:
 * @code
 * #define SERIAL_CONTEXT_SETUP_FUNC myCustomSerialSetup
 * @endcode
 */
#ifndef SERIAL_CONTEXT_SETUP_FUNC
    #ifdef __linux__
        #define SERIAL_CONTEXT_SETUP_FUNC setContextLinux
    #elif __APPLE__
        #define SERIAL_CONTEXT_SETUP_FUNC setContextLinux
    #elif _WIN32
        #define SERIAL_CONTEXT_SETUP_FUNC setContextWindows
    #elif ARDUINO
        #define SERIAL_CONTEXT_SETUP_FUNC setContextArduino
    #endif
#endif

/**
 * @brief Predefined Cloudloop topic identifiers for RockBLOCK 9704.
 *
 * This enum defines known topic IDs used to route messages through the
 * Cloudloop data.
 */
typedef enum
{
    /** Raw topic ID used for unfiltered transmission. */
    RAW_TOPIC = 244,

    /** Topic ID for messages labeled "Purple". */
    PURPLE_TOPIC = 313,

    /** Topic ID for messages labeled "Pink". */
    PINK_TOPIC = 314,

    /** Topic ID for messages labeled "Red". */
    RED_TOPIC = 315,

    /** Topic ID for messages labeled "Orange". */
    ORANGE_TOPIC = 316,

    /** Topic ID for messages labeled "Yellow". */
    YELLOW_TOPIC = 317
} cloudloopTopics_t;


/**
 * @brief Initialise the the serial connection in the detected context (or user defined),
 * if successful continue to set the API, SIM & state of the modem in order
 * to be ready for messaging.
 * 
 * @param port pointer to port name.
 * @return bool depicting success or failure.
 */
bool rbBegin(const char * port);

/**
 * @brief Uninitialise/close the the serial connection.
 * 
 * @return bool depicting success or failure.
 */
bool rbEnd(void);

/**
 * @brief Send a mobile originated message from the modem on the default topic (244).
 * 
 * @param data pointer to data (message).
 * @param length size_t of data length. (Max 100kB).
 * @param timeout in seconds.
 * @return bool depicting success or failure.
 */
bool rbSendMessage(const char * data, const size_t length, const int timeout);

/**
 * @brief Send a mobile originated message from the modem on a cloudloop topic of choice.
 * 
 * @param topic uint16_t topic.
 * @param data pointer to data (message).
 * @param length size_t of data length. (Max 100kB).
 * @param timeout in seconds.
 * @return bool depicting success or failure.
 */
bool rbSendMessageCloudloop(cloudloopTopics_t topic, const char * data, const size_t length, const int timeout);

/**
 * @brief Send a mobile originated message from the modem on any topic.
 * 
 * @param topic uint16_t topic.
 * @param data pointer to data (message).
 * @param length size_t of data length. (Max 100kB).
 * @param timeout in seconds.
 * @return bool depicting success or failure.
 */
bool rbSendMessageAny(uint16_t topic, const char * data, const size_t length, const int timeout);

/**
 * @brief Listen for a mobile terminated message from the modem.
 * 
 * @param buffer pointer to buffer of the stored MT messages.
 * @return size_t the length of the buffer minus the IMT CRC.
 * 
 * * @note this pointer is a pointer to a pointer to the MT queue buffer, 
 * it may be reused to store another MT. It must be copied if the application 
 * code needs to preserve it for a period of time.
 */
size_t rbReceiveMessage(char ** buffer);

/**
 * @brief Listen for a mobile terminated message from the modem.
 * 
 * @param buffer pointer to buffer of the stored MT messages.
 * @param topic uint16_t topic.
 * @return size_t the length of the buffer minus the IMT CRC.
 * 
 * * @note this pointer is a pointer to a pointer to the MT queue buffer, 
 * it may be reused to store another MT. It must be copied if the application 
 * code needs to preserve it for a period of time.
 */
size_t rbReceiveMessageWithTopic(char ** buffer, uint16_t topic);

/**
 * @brief Get the current signal strength from the modem.
 *
 * The signal strength is returned in signal bars from 0 to 5:
 * - 0: No signal
 * - 5: Maximum signal strength
 *
 * @note A return value of -1 indicates an error occurred while communicating 
 * with the modem to retrieve the signal strength.
 *
 * @return int8_t Signal strength in bars (0–5), or -1 on error.
 */
int8_t rbGetSignal(void);

/**
 * @brief Get the hardware version.
 * 
 * @return char pointer to hwVersion string.
 */
char * rbGetHwVersion(void);

/**
 * @brief Get the serial number.
 * 
 * @return char pointer to serial number string.
 */
char * rbGetSerialNumber(void);

/**
 * @brief Get the imei.
 * 
 * @return char pointer to imei string.
 */
char * rbGetImei(void);

/**
 * @brief Get the board temperature.
 * 
 * @return int8_t of the current temperature (-100 on error).
 */
int8_t rbGetBoardTemp(void);

/**
 * @brief Check if SIM presence is currently asserted.
 * 
 * @return bool depicting SIM presence.
 * * @note This function will return false either if it received
 * it from the modem or the function failed.
 */
bool rbGetCardPresent(void);

/**
 * @brief Check if SIM card is present, communicating properly with,
 * and has presented no errors in SIM transactions with the
 * transceiver.
 * 
 * @return bool depicting SIM communicating correctly.
 * * @note This function will return false either if it received
 * it from the modem or the function failed.
 */
bool rbGetSimConnected(void);

/**
 * @brief Get the iccid.
 * 
 * @return char pointer to iccid string.
 */
char * rbGetIccid(void);

/**
 * @brief Get the Iridium modem firmware version as vX.Y.X
 * with X being the major number, Y being the minor number and X
 * being the patch number
 * 
 * @return char pointer to firmware version
 */
char *  rbGetFirmwareVersion(void);

#if defined(KERMIT)
/**
 * @brief A callback definition for the kermit transfer
 * 
 * @param context a pointer to some shared context given in rbUpdateFirmware.
 * @param sofar the number of bytes transferred so far.
 * @param total the total number of bytes to transfer.
 * @return void
 * * @note This is only defined if KERMIT was defined during the build.
 */
typedef void(*updateProgressCallback)(void * context, const unsigned long sofar, const unsigned long total);

/**
 * @brief Update 9704 firmware. This is a blocking call and will take approximately
 * 10 minutes to upgrade, the updateProgressCallback is recommended.
 * 
 * @param firmwareFile path to the sxbin firmware files from Iridium
 * @param progress pointer to the update progress callback, this can be NULL.
 * @param context pointer to to some shared memory to pass to progress, this can be NULL.
 * @return bool true if the upgrade was successful.
 * * @note This is only defined if KERMIT was defined during the build.
 */
bool rbUpdateFirmware (const char * firmwareFile, updateProgressCallback progress, void * context);
#endif

#ifdef RB_GPIO
#include "gpio.h"

/**
 * 
 * @brief Drives user defined pin (power enable) low and user defined pin (iridium enable) high to 
 * initialise the RB9704 PiHat. Initialise the serial connection in the 
 * detected context (or user defined), if successful continue to set 
 * the API, SIM & state of the modem in order to be ready for messaging.
 * 
 * @param port pointer to port name.
 * @param gpioInfo structure containing a valid chip & pin for powerEnable, IridiumEnable and booted.
 * @param timeout in seconds.
 * @return bool depicting success or failure.
 */
bool rbBeginGpio(char * port, const rbGpioTable_t * gpioInfo, const int timeout);

/**
 * @brief Drives user defined pin (power enable) high and another
 * user defined pin (iridium enable) low to deinitialise the RB9704
 * PiHat. Deinitialises/closes the the serial connection.
 * 
 * @param gpioInfo structure containing a valid chip & pin for powerEnable, IridiumEnable and booted.
 * @return bool depicting success or failure.
 */
bool rbEndGpio(const rbGpioTable_t * gpioInfo);
#endif

/**
 * @brief Calculate CRC for a buffer.
 *
 * @param buffer Pointer to the data buffer.
 * @param bufferLength Length of the buffer in bytes.
 * @param initialCRC Initial CRC value to start from.
 * @return Calculated CRC as an uint16_t.
 */
static uint16_t calculateCrc(const uint8_t * buffer, const size_t bufferLength, const uint16_t initialCRC);

/**
 * @brief Append CRC to the end of a buffer.
 *
 * @param buffer Pointer to the buffer to append CRC to.
 * @param length Length of the buffer in bytes.
 * @return true on success, false on failure.
 */
static bool appendCrc(uint8_t * buffer, size_t length);

/**
 * @brief Encode binary data to base64 format.
 *
 * @param srcBuffer Pointer to source binary data.
 * @param srcLength Length of source data in bytes.
 * @param destBuffer Pointer to destination buffer for base64 string.
 * @param destLength Length of destination buffer.
 * @return Number of bytes written to destBuffer.
 */
static size_t encodeData(const char * srcBuffer, const size_t srcLength, char * destBuffer, const size_t destLength);

/**
 * @brief Decode base64 data back into binary.
 *
 * @param srcBuffer Pointer to base64-encoded string.
 * @param srcLength Length of encoded string.
 * @param destBuffer Pointer to destination binary buffer.
 * @param destLength Length of destination buffer.
 * @return Number of bytes written to destBuffer.
 */
static size_t decodeData(const char * srcBuffer, const size_t srcLength, char * destBuffer, const size_t destLength);

/**
 * @brief Set the modem API version.
 *
 * @return true if successful, false otherwise.
 */
static bool setApi(void);

/**
 * @brief Initialise SIM card configuration.
 *
 * @return true if SIM setup succeeded, false otherwise.
 */
static bool setSim(void);

/**
 * @brief Set operational state for the modem.
 *
 * @return true if successful, false otherwise.
 */
static bool setState(void);

/**
 * @brief Check if the given topic is provisioned.
 *
 * @param topic Topic ID to check.
 * @return true if the topic is provisioned, false otherwise.
 */
static bool checkProvisioning(uint16_t topic);

/**
 * @brief Send a queued message using mobile originated (MO) transmission.
 *
 * @param timeout Timeout duration in seconds.
 * @return true if message was sent successfully, false otherwise.
 */
static bool sendMoFromQueue(const int timeout);

/**
 * @brief Listen for an incoming mobile terminated (MT) message.
 *
 * @return true if a message was received, false otherwise.
 */
static bool listenForMt(void);

/**
 * @brief Retrieve hardware information from the modem.
 *
 * @param hwInfo Pointer to structure to populate with hardware info.
 * @return true on success, false on failure.
 */
static bool getHwInfo(jsprHwInfo_t * hwInfo);

/**
 * @brief Get current SIM card status.
 *
 * @param simStatus Pointer to structure to populate with SIM status.
 * @return true on success, false on failure.
 */
static bool getSimStatus(jsprSimStatus_t * simStatus);

#ifdef __cplusplus
}
#endif

#endif