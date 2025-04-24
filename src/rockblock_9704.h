#ifndef ROCKBLOCK_9704_H
#define ROCKBLOCK_9704_H

#ifdef __cplusplus
extern "C" {
#endif

#include "serial.h"
#include "jspr.h"
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stddef.h>

#define BASE64_TEMP_BUFFER 2048U
#define RB9704_BAUD 230400U

typedef enum
{
    RAW_TOPIC = 244, 
    PURPLE_TOPIC = 313, 
    PINK_TOPIC = 314, 
    RED_TOPIC = 315,
    ORANGE_TOPIC = 316, 
    YELLOW_TOPIC = 317
}cloudloopTopics_t;

//user functions

/**
 * Initialise the the serial connection in the detected context (or user defined),
 * if successful continue to set the API, SIM & state of the modem in order
 * to be ready for messaging.
 * 
 * @param port pointer to port name.
 * @return bool depicting success or failure.
 */
bool rbBegin(char * port);

/**
 * Uninitialise/close the the serial connection.
 * 
 * @return bool depicting success or failure.
 */
bool rbEnd(void);

/**
 * Send a mobile originated message from the modem on the default topic (244).
 * 
 * @param data pointer to data (message).
 * @param length size_t of data length. (Max 100kB).
 * @return bool depicting success or failure.
 */
bool sendMessage(const char * data, const size_t length);

/**
 * Send a mobile originated message from the modem on a cloudloop topic of choice.
 * 
 * @param topic uint16_t topic.
 * @param data pointer to data (message).
 * @param length size_t of data length. (Max 100kB).
 * @return bool depicting success or failure.
 */
bool sendMessageCloudloop(cloudloopTopics_t topic, const char * data, const size_t length);

/**
 * Send a mobile originated message from the modem on any topic.
 * 
 * @param topic uint16_t topic.
 * @param data pointer to data (message).
 * @param length size_t of data length. (Max 100kB).
 * @return bool depicting success or failure.
 */
bool sendMessageAny(uint16_t topic, const char * data, const size_t length);

/**
 * Listen for a mobile terminated message from the modem.
 * 
 * @param buffer pointer to buffer of the stored MT messages
 * @return size_t the length of the buffer minus the IMT CRC
 * 
 * * @note this pointer is a pointer to a pointer to the MT queue buffer, 
 * it may be reused to store another MT. It must be copied if the application 
 * code needs to preserve it for a period of time.
 */
size_t receiveMessage(char ** buffer);

/**
 * Listen for a mobile terminated message from the modem.
 * 
 * @param buffer pointer to buffer of the stored MT messages
 * @param length uint16_t to store the topic of the incoming message.
 * @return size_t the length of the buffer minus the IMT CRC
 * 
 * * @note this pointer is a pointer to a pointer to the MT queue buffer, 
 * it may be reused to store another MT. It must be copied if the application 
 * code needs to preserve it for a period of time.
 */
size_t receiveMessageWithTopic(char ** buffer, uint16_t topic);

/**
 * Get the current signal strength (0-5) from the modem.
 * 
 * @return uint8_t of the signal strength from the modem (-1 on error).
 */
int8_t getSignal(void);

/**
 * Get the hardware version.
 * 
 * @return char pointer to hwVersion string.
 */
char * getHwVersion(void);

/**
 * Get the serial number.
 * 
 * @return char pointer to serial number string.
 */
char * getSerialNumber(void);

/**
 * Get the imei.
 * 
 * @return char pointer to imei string.
 */
char * getImei(void);

/**
 * Get the board temperature.
 * 
 * @return int8_t of the current temperature (-100 on error).
 */
int8_t getBoardTemp(void);

/**
 * Check if SIM presence is currently asserted.
 * 
 * @return bool depicting SIM presence.
 * * @note This function will return false either if it received
 * it from the modem or the function failed.
 */
bool getCardPresent(void);

/**
 * Check if SIM card is present, communicating properly with,
 * and has presented no errors in SIM transactions with the
 * transceiver.
 * 
 * @return bool depicting SIM communicating correctly.
 * * @note This function will return false either if it received
 * it from the modem or the function failed.
 */
bool getSimConnected(void);

/**
 * Get the iccid.
 * 
 * @return char pointer to iccid string.
 */
char * getIccid(void);

/**
 * Get the Iridium modem firmware version as vX.Y.X
 * with X being the major number, Y being the minor number and X
 * being the patch number
 * 
 * @return char pointer to firmware version
 */
char *  getFirmwareVersion(void);

#if defined(KERMIT)
typedef void(*updateProgressCallback)(const unsigned long sofar, const unsigned long total);
bool updateFirmware (const char * firmwareFile, updateProgressCallback progress);
#endif

#ifdef RB_GPIO
#include "gpio.h"

/**
 * 
 * Drives user defined pin (power enable) low and user defined pin (iridium enable) high to 
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
 * Drives user defined pin (power enable) high and another
 * user defined pin (iridium enable) low to deinitialise the RB9704
 * PiHat. Uninitialises/closes the the serial connection.
 * 
 * @param gpioInfo structure containing a valid chip & pin for powerEnable, IridiumEnable and booted.
 * @return bool depicting success or failure.
 */
bool rbEndGpio(const rbGpioTable_t * gpioInfo);

/**
 * 
 * Drives pin 24 (power enable) low and pin 16
 * (iridium enable) high to initialise the RB9704
 * PiHat. Initialises the the serial connection in the detected context 
 * (or user defined), if successful continue to set the API, SIM & 
 * state of the modem in order to be ready for messaging.
 * 
 * @param timeout in seconds.
 * @return bool depicting success or failure.
 */
bool rbBeginHat(const int timeout);

/**
 * 
 * Drives pin 24 (power enable) high and pin 16
 * (iridium enable) low to deinitialise the RB9704
 * PiHat. Uninitialises/closes the the serial connection.
 * 
 * @return bool depicting success or failure.
 */
bool rbEndHat(void);
#endif

//internal functions
static int calculateCrc(const void * buffer, int bufferLength, int initialCRC);
static bool appendCrc(uint8_t * buffer, size_t length);
static size_t encodeData(const char * srcBuffer, const size_t srcLength, char * destBuffer, const size_t destLength);
static size_t decodeData(const char * srcBuffer, const size_t srcLength, char * destBuffer, const size_t destLength);
static bool setApi(void);
static bool setSim(void);
static bool setState(void);
static bool checkProvisioning(uint16_t topic);
static bool sendMoFromQueue(void);
static bool listenForMt(void);
static bool getHwInfo(jsprHwInfo_t * hwInfo);
static bool getSimStatus(jsprSimStatus_t * simStatus);

#ifdef __cplusplus
}
#endif

#endif