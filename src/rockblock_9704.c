#include "rockblock_9704.h"
#include "jspr_command.h"
#include "serial.h"
#include "imt_queue.h"

#include "third_party/cJSON/cJSON.h"
#include "third_party/base64/base64.h"
#include <stddef.h>
#include <errno.h>
#include <stdlib.h>
#include "crossplatform.h"

#if defined(_WIN32)
#include <io.h>
#define access _access
#else
#include <unistd.h>
#endif

#define IMT_MIN_TOPIC_ID 64U
#define IMT_MAX_TOPIC_ID 65535U
#define FIRMWARE_VERSION_STRING_LEN 13U

#ifndef SERIAL_CONTEXT_SETUP_FUNC
    #error A serial context function is needed
#endif

extern int messageReference;
extern serialContext context;
extern enum serialState serialState;

static uint8_t base64Buffer [BASE64_TEMP_BUFFER];
static uint8_t crcBuffer [IMT_CRC_SIZE];

static char firmwareVersion [FIRMWARE_VERSION_STRING_LEN];

jsprHwInfo_t hwInfo;
jsprSimStatus_t simStatus;
jsprFirmwareInfo_t firmwareInfo;
jsprMessageProvisioning_t messageProvisioningInfo;
static jsprResponse_t response;

uint32_t messageLengthAsync = 0;
uint16_t moQueuedMessages = 0;
bool Receivelock = false;
bool moDropped = false;
bool moSent = false;
bool mtDropped = false;
bool mtReceived = false;

static const rbCallbacks_t *rbCallbacks = NULL;

void rbRegisterCallbacks(const rbCallbacks_t *callbacks) 
{
    if (callbacks) 
    {
        rbCallbacks = callbacks;
    }
}

#ifdef RB_GPIO
bool rbBeginGpio(char * port, const rbGpioTable_t * gpioInfo, const int timeout)
{
    bool enabled = false;
    if (gpioDriveLow(gpioInfo->powerEnable.chip, gpioInfo->powerEnable.pin))
    {
        if (gpioDriveHigh(gpioInfo->iridiumEnable.chip, gpioInfo->iridiumEnable.pin))
        {
            if (gpioListenIridBooted(gpioInfo->booted.chip, gpioInfo->booted.pin, timeout))
            {
                sleep(1);
                if (rbBegin(port))
                {
                    enabled = true;
                }
            }
        }
    }
    return enabled;
}

bool rbEndGpio(const rbGpioTable_t * gpioInfo)
{
    bool disabled = false;
    if (gpioDriveHigh(gpioInfo->powerEnable.chip, gpioInfo->powerEnable.pin))
    {
        if (gpioDriveLow(gpioInfo->iridiumEnable.chip, gpioInfo->iridiumEnable.pin))
        {
            if (rbEnd())
            {
                disabled = true;
            }
        }
    }
    return disabled;
}
#endif

static const uint16_t CRC16Table[256] =
{
  0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
  0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
  0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,
  0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de,
  0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485,
  0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
  0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4,
  0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc,
  0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
  0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b,
  0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12,
  0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
  0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41,
  0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49,
  0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
  0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78,
  0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f,
  0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
  0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e,
  0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256,
  0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
  0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
  0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c,
  0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
  0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab,
  0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3,
  0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
  0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92,
  0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9,
  0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
  0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8,
  0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0
};

static bool setApi(void)
{
    bool set = false;
    for(int i = 0; i < 2; i++)
    {
#ifdef ARDUINO
        delay(5);
#else
        usleep(5000);
#endif
        if(jsprGetApiVersion())
        {
            if (receiveJspr(&response, "apiVersion"))
            {
                if(JSPR_RC_NO_ERROR == response.code)
                {
                    jsprApiVersion_t apiVersion;
                    parseJsprGetApiVersion(response.json, &apiVersion);
                    if(!apiVersion.activeVersionSet)
                    {
                        jsprPutApiVersion(&apiVersion.supportedVersions[0]);
                        receiveJspr(&response, "apiVersion");
                    }
                    if(JSPR_RC_NO_ERROR == response.code || apiVersion.activeVersionSet)
                    {
                        set = true;
                        i = 2;
                    }
                }
            }
        }
    }
    return set;
}

static bool setSim(void)
{
    bool set = false;
    if(jsprGetSimInterface())
    {
        if (receiveJspr(&response, "simConfig"))
        {
            if(JSPR_RC_NO_ERROR == response.code)
            {
                jsprSimInterface_t simInterface;
                parseJsprGetSimInterface(response.json, &simInterface);
                
                if(!simInterface.ifaceSet || simInterface.iface != SIM_INTERNAL)
                {
                    putSimInterface(SIM_INTERNAL);
                    receiveJspr(&response, "simConfig");
                    if ((JSPR_RC_NO_ERROR == response.code) &&
                        (strncmp(response.target, "simConfig", JSPR_MAX_TARGET_LENGTH) == 0))
                    {
                        parseJsprGetSimInterface(response.json, &simInterface);

                        // Wait for unsolicited simStatus to come back
                        if (waitForJsprMessage(&response, "simStatus", JSPR_RC_UNSOLICITED_MESSAGE, 1) == true)
                        {
                            set = true;
                        }
                    }
                }
                else if (JSPR_RC_NO_ERROR == response.code && simInterface.iface == SIM_INTERNAL)
                {
                    set = true;
                }
            }
        }
    }
    return set;
}

static bool setState(void)
{
    bool set = false;
    if(jsprGetOperationalState())
    {
        if(receiveJspr(&response, "operationalState"))
        {
            if(JSPR_RC_NO_ERROR == response.code)
            {
                jsprOperationalState_t state;
                parseJsprGetOperationalState(response.json, &state);
                if(state.operationalStateSet)
                {
                    if(state.operationalState == ACTIVE)
                    {
                        set = true;
                    }
                    else if(state.operationalState == INACTIVE)
                    {
                        putOperationalState(ACTIVE);
                        receiveJspr(&response, "operationalState");
                        if(JSPR_RC_NO_ERROR == response.code)
                        {
                            set = true;
                        }
                    }
                    else //if its in another mode it may need to be turned inactive first
                    {
                        putOperationalState(INACTIVE);
                        receiveJspr(&response, "operationalState");
                        if(JSPR_RC_NO_ERROR == response.code)
                        {
                            putOperationalState(ACTIVE);
                            receiveJspr(&response, "operationState");
                            if(JSPR_RC_NO_ERROR == response.code)
                            {
                                set = true;
                            }
                        }
                    }
                }
            }
        }
    }
    return set;
}

bool rbBegin(const char* port)
{
    bool began = false;
    if(SERIAL_CONTEXT_SETUP_FUNC(port, RB9704_BAUD))
    {
        if(context.serialInit != NULL)
        {
            if(context.serialInit())
            {
                serialState = OPEN;
                if(setApi())
                {
                    if(setSim())
                    {
                        if(setState())
                        {
                            imtQueueInit(); //initialise (clean) the queue
                            began = true;
                        }
                    }
                }
            }
        }
    }
    return began;
}

static size_t encodeData(const char * srcBuffer, const size_t srcLength, char * destBuffer, const size_t destLength)
{
    size_t encodedBytes = -1;
    if(srcBuffer != NULL && srcLength > 0 && destBuffer != NULL && destLength > 0)
    {
        int err = mbedtls_base64_encode(destBuffer, destLength, &encodedBytes, srcBuffer, srcLength);
        if (0 != err)
        {
            encodedBytes = -1;
        }
    }
    return encodedBytes;
}

static size_t decodeData(const char * srcBuffer, const size_t srcLength, char * destBuffer, const size_t destLength)
{
    size_t decodedBytes = -1;
    if(srcBuffer != NULL && srcLength > 0 && destBuffer != NULL && destLength > 0)
    {
        int err = mbedtls_base64_decode(destBuffer, destLength, &decodedBytes, srcBuffer, srcLength);
        if (0 != err)
        {
            decodedBytes = -1;
        }
    }
    return decodedBytes;
}

static bool appendCrc(uint8_t * buffer, size_t length)
{
    bool appended = false;
    uint16_t crc = calculateCrc(buffer, length, 0);
    if (crc > 0)
    {
        crcBuffer[0] = (crc >> 8) & 0xFFU;
        crcBuffer[1] = crc & 0xFFU;
        memcpy(buffer + length, crcBuffer, IMT_CRC_SIZE);
        appended = true;
    }
    memset(crcBuffer, 0, IMT_CRC_SIZE);
    return appended;
}

bool rbSendMessage(const char * data, const size_t length, const int timeout)
{
    bool sent = false;
    bool queued = false;
    if(checkProvisioning(RAW_TOPIC))
    {
        if (moQueuedMessages > 0)
        {
            imtQueueMoRemove();
            moQueuedMessages--;
        }
        if(data != NULL && length > 0 && length <= IMT_PAYLOAD_SIZE - IMT_CRC_SIZE)
        {
            queued = imtQueueMoAdd(RAW_TOPIC, data, length);
            if(queued)
            {
                sent = sendMoFromQueue(timeout);
            }
        }
    }
    return sent;
}

bool rbSendMessageCloudloop(cloudloopTopics_t topic, const char * data, const size_t length, const int timeout)
{
    bool sent = false;
    bool queued = false;
    if(checkProvisioning(topic))
    {
        if (moQueuedMessages > 0)
        {
            imtQueueMoRemove();
            moQueuedMessages--;
        }
        if(data != NULL && length > 0 && length <= IMT_PAYLOAD_SIZE - IMT_CRC_SIZE)
        {
            queued = imtQueueMoAdd(topic, data, length);
            if(queued)
            {
                sent = sendMoFromQueue(timeout);
            }
        }
    }
    return sent;
}

bool rbSendMessageAny(uint16_t topic, const char * data, const size_t length, const int timeout)
{
    bool sent = false;
    bool queued = false;
    if(checkProvisioning(topic))
    {
        if (moQueuedMessages > 0)
        {
            imtQueueMoRemove();
            moQueuedMessages--;
        }
        if(data != NULL && length > 0 && length <= IMT_PAYLOAD_SIZE - IMT_CRC_SIZE)
        {
            queued = imtQueueMoAdd(topic, data, length);
            if(queued >= 0)
            {
                sent = sendMoFromQueue(timeout);
            }
        }
    }
    return sent;
}

static bool sendMoFromQueue(const int timeout)
{
    bool sent = false;
    bool started = false;
    unsigned long start = millis();
    int initCrc = 0;
    int segmentStart;
    int segmentLength;
    int encodedBytes;
    imt_t * imtMo = imtQueueMoGetFirst();

    if(imtMo != NULL)
    {
        if(appendCrc(imtMo->buffer, imtMo->length))
        {
            if(imtMo->buffer != NULL && imtMo->length > 0 && imtMo->topic >= IMT_MIN_TOPIC_ID 
            && imtMo->topic <= IMT_MAX_TOPIC_ID)
            {
                if(jsprPutMessageOriginate(imtMo->topic, imtMo->length + IMT_CRC_SIZE))
                {
                    if(receiveJspr(&response, "messageOriginate"))
                    {
                        if(JSPR_RC_NO_ERROR == response.code)
                        {
                            jsprMessageOriginate_t messageOriginate;
                            parseJsprPutMessageOriginate(response.json, &messageOriginate);
                            imtMo->id = messageOriginate.messageId;
                            started = true;
                            while (true)
                            {
                                rbPoll();
                                if(moDropped)
                                {
                                    sent = false;
                                    moDropped = false;
                                    break;
                                }
                                else if (moSent)
                                {
                                    sent = true;
                                    moSent = false;
                                    break;
                                }
                                else if ((millis() - start) >= (timeout * 1000UL))
                                {
                                    sent = false;
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }

        if(!started)
        {
            imtQueueMoRemove(); //failed one of the checks, drop message
        }
    }
    return sent;
}

size_t rbReceiveMessage(char ** buffer)
{
    size_t length = 0;

    if(listenForMt())
    {
        imt_t * imtMt = imtQueueMtGetFirst();
        if(buffer != NULL && imtMt != NULL)
        {
            if(imtMt->buffer != NULL && imtMt->length > 0 && imtMt->topic >= IMT_MIN_TOPIC_ID &&
                imtMt->topic <= IMT_MAX_TOPIC_ID) //check head is valid mt
            {
                length = (imtMt->length - IMT_CRC_SIZE);
                imtMt->buffer[length] = '\0'; //remove crc
                *buffer = imtMt->buffer;
                imtMt->readyToProcess = false; //finished processing
            }
        }
    }
    return length;
}

size_t rbReceiveMessageWithTopic(char ** buffer, uint16_t topic)
{
    size_t length = 0;

    if(listenForMt())
    {
        imt_t * imtMt = imtQueueMtGetFirst();
        if(buffer != NULL && imtMt != NULL)
        {
            if(imtMt->buffer != NULL && imtMt->length > 0 && imtMt->topic >= IMT_MIN_TOPIC_ID &&
                imtMt->topic <= IMT_MAX_TOPIC_ID) //check head is valid mt
            {
                length = (imtMt->length - IMT_CRC_SIZE);
                imtMt->buffer[length] = '\0'; //remove crc
                *buffer = imtMt->buffer;
                topic = imtMt->topic;
                imtMt->readyToProcess = false; //finished processing
            }
        }
    }
    return length;
}

static bool listenForMt(void)
{
    bool received = false;

    rbPoll();
    imt_t * imtMt = imtQueueMtGetFirst();
    if(imtMt != NULL)
    {
        if(imtMt->readyToProcess)
        {
            while(true)
            {
                rbPoll();
                if(mtDropped)
                {
                    received = false;
                    mtDropped = false;
                    break;
                }
                else if(mtReceived)
                {
                    received = true;
                    mtReceived = false;
                    break;
                }
            }
        }
    }
    return received;
}

static bool sendMoFromQueueAsync(void)
{
    bool started = false;
    imt_t * imtMo = imtQueueMoGetFirst();

    if(imtMo != NULL)
    {
        if(appendCrc(imtMo->buffer, imtMo->length))
        {
            if(imtMo->buffer != NULL && imtMo->length > 0 && imtMo->topic >= IMT_MIN_TOPIC_ID 
            && imtMo->topic <= IMT_MAX_TOPIC_ID)
            {
                if(jsprPutMessageOriginate(imtMo->topic, imtMo->length + IMT_CRC_SIZE))
                {
                    if(receiveJspr(&response, "messageOriginate"))
                    {
                        if(JSPR_RC_NO_ERROR == response.code)
                        {
                            jsprMessageOriginate_t messageOriginate;
                            parseJsprPutMessageOriginate(response.json, &messageOriginate);
                            imtMo->id = messageOriginate.messageId;
                            started = true;
                        }
                    }
                }
            }
        }

        if(!started)
        {
            imtQueueMoRemove(); //failed one of the checks, drop message
        }
    }
    return started;
}

bool rbSendMessageAsync(uint16_t topic, const char * data, const size_t length)
{
    bool queuedToSend = false;
    bool queued = false;
    if(checkProvisioning(topic))
    {
        if(data != NULL && length > 0 && length <= IMT_PAYLOAD_SIZE - IMT_CRC_SIZE)
        {
            queued = imtQueueMoAdd(topic, data, length);
            if(queued)
            {
                if (moQueuedMessages == 0)
                {
                    queuedToSend = sendMoFromQueueAsync();
                }
                else
                {
                    queuedToSend = true;
                }
                moQueuedMessages += 1;
            }
        }
    }
    return queuedToSend;
}

size_t rbReceiveMessageAsync(char ** buffer)
{
    size_t length = 0;
    imt_t * imtMt = imtQueueMtGetFirst();

    if(imtMt != NULL)
    {
        if(imtMt->ready)
        {
            if(buffer != NULL)
            {
                if(imtMt->buffer != NULL && imtMt->length > 0 && imtMt->topic >= IMT_MIN_TOPIC_ID &&
                    imtMt->topic <= IMT_MAX_TOPIC_ID && imtMt->ready) //check head is valid mt
                {
                    length = (imtMt->length - IMT_CRC_SIZE);
                    imtMt->buffer[length] = '\0'; //remove crc
                    *buffer = imtMt->buffer;
                    imtMt->readyToProcess = false; //finished processing
                }
            }
        }
    }
    return length;
}

void rbReceiveLockAsync(void)
{
    imtQueueMtLock(true);
}

void rbReceiveUnlockAsync(void)
{
    imtQueueMtLock(false);
}

void rbSendLockAsync(void)
{
    imtQueueMoLock(true);
}

void rbSendUnlockAsync(void)
{
    imtQueueMoLock(false);
}

bool rbAcknowledgeReceiveHeadAsync(void)
{
    bool acknowledged = false;
    if(imtQueueMtRemove())
    {
        acknowledged = true;
    }
    return acknowledged;
}

static bool checkMoQueue(void)
{
    bool success = false;
    if(moQueuedMessages > 0) //check if any more messages are queued
    {
        if(sendMoFromQueueAsync()) //send the next message
        {
            success = true;
        }
    }
    return success;
}

void rbPoll(void)
{
    int segmentStart;
    int segmentStartMt;
    int segmentLength;
    int segmentLengthMt;
    int encodedBytes;
    int decodedBytes;
    bool mtQueued;
    imt_t * imtMo = imtQueueMoGetFirst();
    if(context.serialPeek() > 0)
    {
        if(receiveJspr(&response, NULL))
        {
            //MO JSPR
            if(imtMo != NULL)
            {
                if(JSPR_RC_UNSOLICITED_MESSAGE == response.code && strcmp(response.target, "messageOriginateSegment") == 0)
                {
                    jsprMessageOriginateSegment_t messageOriginateSegment;
                    parseJsprUnsMessageOriginateSegment(response.json, &messageOriginateSegment);
                    if(messageOriginateSegment.messageId == imtMo->id && 
                    messageOriginateSegment.topic == imtMo->topic)
                    {
                        segmentStart = messageOriginateSegment.segmentStart;
                        segmentLength = messageOriginateSegment.segmentLength;
                        encodedBytes = encodeData(imtMo->buffer + segmentStart, 
                        segmentLength, base64Buffer, BASE64_TEMP_BUFFER);
                        if(0 < encodedBytes)
                        {
                            jsprMessageOriginate_t messageOriginate;
                            messageOriginate.messageId = imtMo->id;
                            messageOriginate.topic = imtMo->topic;
                            jsprPutMessageOriginateSegment(&messageOriginate, segmentLength, 
                            segmentStart, base64Buffer);
                        }
                    }
                }
                if(JSPR_RC_NO_ERROR != response.code && JSPR_RC_UNSOLICITED_MESSAGE != response.code && strcmp(response.target, "messageOriginateSegment") == 0)
                {
                    jsprMessageOriginateSegment_t messageOriginateSegment;
                    if(parseJsprUnsMessageOriginateSegment(response.json, &messageOriginateSegment))
                    {
                        if(imtMo->id == messageOriginateSegment.messageId)
                        {
                    
                            if(rbCallbacks && rbCallbacks->moMessageComplete)
                            {
                                rbCallbacks->moMessageComplete(imtMo->id, RB_MSG_STATUS_FAIL);
                            }
                            else
                            {
                                moDropped = true;
                            }
                            imtQueueMoRemove(); //drop message
                            moQueuedMessages -= 1;
                            checkMoQueue();
                        }
                    }
                }
                if(JSPR_RC_UNSOLICITED_MESSAGE == response.code && strcmp(response.target, "messageOriginateStatus") == 0)
                {
                    jsprMessageOriginateStatus_t messageOriginateStatus;
                    if(parseJsprUnsMessageOriginateStatus(response.json, &messageOriginateStatus))
                    {
                        if(imtMo->id == messageOriginateStatus.messageId)
                        {
                            if(messageOriginateStatus.finalMoStatus == MO_ACK_RECEIVED_MOS)
                            {
                                if(rbCallbacks && rbCallbacks->moMessageComplete)
                                {
                                    rbCallbacks->moMessageComplete(imtMo->id, RB_MSG_STATUS_OK);
                                }
                                else
                                {
                                    moSent = true;
                                }
                            }
                            else
                            {
                                if(rbCallbacks && rbCallbacks->moMessageComplete)
                                {
                                    rbCallbacks->moMessageComplete(imtMo->id, RB_MSG_STATUS_FAIL);
                                }
                                else
                                {
                                    moDropped = true;
                                }
                            }
                            imtQueueMoRemove();
                            moQueuedMessages -= 1;
                            checkMoQueue();
                        }
                    }
                }
            }
            //MT JSPR
            if(JSPR_RC_UNSOLICITED_MESSAGE == response.code && strcmp(response.target, "messageTerminate") == 0)
            {
                jsprMessageTerminate_t messageTerminate;
                parseJsprUnsMessageTerminate(response.json, &messageTerminate);
                mtQueued = imtQueueMtAdd(messageTerminate.topic, messageTerminate.messageId, messageTerminate.messageLengthMax);
                imt_t * imtMt = imtQueueMtGetLast();
                if (mtQueued) //returns -1 if que is full, no free spots to store mt
                {
                    if(imtMt != NULL)
                    {
                        imtMt->readyToProcess = true;
                    }
                }
                else
                {
                    if(rbCallbacks && rbCallbacks->mtMessageComplete)
                    {
                        rbCallbacks->mtMessageComplete(messageTerminate.messageId, RB_MSG_STATUS_FAIL);
                    }
                }
            }
            if(JSPR_RC_UNSOLICITED_MESSAGE == response.code && strcmp(response.target, "messageTerminateSegment") == 0)
            {
                imt_t * imtMt = imtQueueMtGetLast();
                if(imtMt != NULL)
                {
                    if(imtMt->readyToProcess)
                    {
                        jsprMessageTerminateSegment_t messageTerminateSegment;
                        parseJsprUnsMessageTerminateSegment(response.json, &messageTerminateSegment);
                        segmentStartMt = messageTerminateSegment.segmentStart;
                        segmentLengthMt = messageTerminateSegment.segmentLength;
                        if(imtMt->id == messageTerminateSegment.messageId)
                        {
                            decodedBytes = decodeData(messageTerminateSegment.data, messageTerminateSegment.dataLength, 
                            imtMt->buffer + segmentStartMt, segmentLengthMt);
                            messageLengthAsync += segmentLengthMt;
                            if(0 > decodedBytes)
                            {
                                if(rbCallbacks && rbCallbacks->mtMessageComplete)
                                {
                                    rbCallbacks->mtMessageComplete(imtMt->id, RB_MSG_STATUS_FAIL);
                                }
                                else
                                {
                                    mtDropped = true;
                                }
                                imtQueueMtRemove();
                            }
                        }
                    }
                }
            }
            if(JSPR_RC_UNSOLICITED_MESSAGE == response.code && strcmp(response.target, "messageTerminateStatus") == 0)
            {
                imt_t * imtMt = imtQueueMtGetLast();
                if(imtMt != NULL)
                {
                    if(imtMt->readyToProcess)
                    {
                        jsprMessageTerminateStatus_t messageTerminateStatus;
                        if(parseJsprUnsMessageTerminateStatus(response.json, &messageTerminateStatus))
                        {
                            if(imtMt->id == messageTerminateStatus.messageId)
                            {
                                if(messageTerminateStatus.finalMtStatus == COMPLETE)
                                {
                                    imtMt->length = messageLengthAsync;
                                    messageLengthAsync = 0;
                                    imtMt->ready = true;
                                    if(rbCallbacks && rbCallbacks->mtMessageComplete)
                                    {
                                        rbCallbacks->mtMessageComplete(imtMt->id, RB_MSG_STATUS_OK);
                                    }
                                    else
                                    {
                                        mtReceived = true;
                                    }
                                }
                                else
                                {
                                    if(rbCallbacks && rbCallbacks->mtMessageComplete)
                                    {
                                        rbCallbacks->mtMessageComplete(imtMt->id, RB_MSG_STATUS_FAIL);
                                    }
                                    else
                                    {
                                        mtDropped = true;
                                    }
                                }
                            }
                        }
                    }
                }
            }
            if(JSPR_RC_UNSOLICITED_MESSAGE == response.code && strcmp(response.target, "constellationState") == 0)
            {
                jsprConstellationState_t constellationState;
                if(parseJsprGetSignal(response.json, &constellationState))
                {
                    if(rbCallbacks && rbCallbacks->constellationState)
                    {
                        rbCallbacks->constellationState(&constellationState);
                    }
                }
            }
        }
    }
}

int8_t rbGetSignal(void)
{
    int8_t signal = -1;
    jsprGetSignal();
    waitForJsprMessage(&response, "constellationState", JSPR_RC_NO_ERROR, 1);
    if(JSPR_RC_NO_ERROR == response.code && strcmp(response.target, "constellationState") == 0)
    {
        jsprConstellationState_t conState;
        if(parseJsprGetSignal(response.json, &conState))
        {
            if(conState.signalBars >= 0 && conState.signalBars <= 5)
            {
                signal = conState.signalBars;
            }
        }
    }
    return signal;
}

static bool getHwInfo(jsprHwInfo_t * hwInfo)
{
    bool populated = false;
    jsprGetHwInfo();
    receiveJspr(&response, "hwInfo");
    if(JSPR_RC_NO_ERROR == response.code && strcmp(response.target, "hwInfo") == 0)
    {
        if(parseJsprGetHwInfo(response.json, hwInfo))
        {
            populated = true;
        }
    }
    return populated;
}

char * rbGetImei(void)
{
    char * imei = NULL;
    if(getHwInfo(&hwInfo))
    {
        imei = hwInfo.imei;
    }
    return imei;
}

char * rbGetHwVersion(void)
{
    char * hwVersion = NULL;
    if(getHwInfo(&hwInfo))
    {
        hwVersion = hwInfo.hwVersion;
    }
    return hwVersion;
}

char * rbGetSerialNumber(void)
{
    char * serialNumber = NULL;
    if(getHwInfo(&hwInfo))
    {
        serialNumber = hwInfo.serialNumber;
    }
    return serialNumber;
}

int8_t rbGetBoardTemp(void)
{
    int8_t boardTemp = -100; //needs to be some value that the temp can't be
    jsprHwInfo_t hwInfo;
    if(getHwInfo(&hwInfo))
    {
        boardTemp = hwInfo.boardTemp;
    }
    return boardTemp;
}

static bool getSimStatus(jsprSimStatus_t * simStatus)
{
    bool populated = false;
    jsprGetSimStatus();
    receiveJspr(&response, "simStatus");
    if(JSPR_RC_NO_ERROR == response.code && strcmp(response.target, "simStatus") == 0)
    {
        if(parseJsprGetSimStatus(response.json, simStatus))
        {
            populated = true;
        }
    }
    return populated;
}

bool rbGetCardPresent(void)
{
    bool cardPresent = false;
    if(getSimStatus(&simStatus))
    {
        cardPresent = simStatus.cardPresent;
    }
    return cardPresent;
}

bool rbGetSimConnected(void)
{
    bool simConnected = false;
    if(getSimStatus(&simStatus))
    {
        simConnected = simStatus.simConnected;
    }
    return simConnected;
}

char * rbGetIccid(void)
{
    char * iccid = NULL;
    if(getSimStatus(&simStatus))
    {
        iccid = simStatus.iccid;
    }
    return iccid;
}

static bool getFirmwareInfo(jsprFirmwareInfo_t * fwInfo)
{
    bool populated = false;
    jsprGetFirmware(JSPR_BOOT_SOURCE_PRIMARY);
    receiveJspr(&response, "firmware");
    if(JSPR_RC_NO_ERROR == response.code && strcmp(response.target, "firmware") == 0)
    {
        if(parseJsprFirmwareInfo(response.json, fwInfo))
        {
            populated = true;
        }
    }
    else
    {
        printf("Failed\n");
    }
    return populated;
}

char * rbGetFirmwareVersion(void)
{
    if(getFirmwareInfo(&firmwareInfo))
    {
        snprintf(firmwareVersion, FIRMWARE_VERSION_STRING_LEN,"v%u.%u.%u",
            firmwareInfo.versionInfo.version.major, 
            firmwareInfo.versionInfo.version.minor,
            firmwareInfo.versionInfo.version.patch);
    }
    else
    {
        firmwareVersion[0] = '\0';
    }

    return firmwareVersion;
}

bool rbResyncServiceConfig(void)
{
    bool rVal = false;
    bool isInactive = false;
    bool wasActive = false;
    jsprOperationalState_t state;

    if(jsprGetOperationalState())
    {
        // Wait for 200 Operational State
        if (waitForJsprMessage(&response, "operationalState", JSPR_RC_NO_ERROR, 1) == true)
        {
            parseJsprGetOperationalState(response.json, &state);
            if (state.operationalState == INACTIVE)
            {
                isInactive = true;
            }
            else if (state.operationalState == ACTIVE)
            {
                wasActive = true;
                putOperationalState(INACTIVE);
                // Look for 299 Operational State, this indicates it is actually inactive
                if (waitForJsprMessage(&response, "operationalState", JSPR_RC_UNSOLICITED_MESSAGE, 1) == true)
                {
                    parseJsprGetOperationalState(response.json, &state);
                    isInactive = state.operationalState == INACTIVE;
                }
            }
        }
    }

    if (isInactive == true)
    {
        if (jsprPutServiceConfig(true) == true)
        {
            if (waitForJsprMessage(&response, "serviceConfig", JSPR_RC_NO_ERROR, 1) == true)
            {
                if (wasActive != true)
                {
                    rVal = true;
                }
                else
                {
                    putOperationalState(ACTIVE);
                    // Look for 299 Operational State, this indicates it is actually active again
                    if (waitForJsprMessage(&response, "operationalState", JSPR_RC_UNSOLICITED_MESSAGE, 1) == true)
                    {
                        parseJsprGetOperationalState(response.json, &state);
                        rVal = (state.operationalState == ACTIVE);
                    }
                }
            }
        }
    }

    return rVal;
}

static uint16_t calculateCrc(const uint8_t * buffer, const size_t bufferLength, const uint16_t initialCRC)
{
    uint16_t crc = (uint16_t)initialCRC;
    uint8_t data = 0;
    size_t tableIndex = 0;
    if (buffer != 0)
    {
        for (size_t i = 0; i < bufferLength; i++)
        {
            data = ((uint8_t *)buffer)[i];
            tableIndex = (((crc >> 8) ^ data) & 0xFF);
            crc = (((crc << 8) ^ CRC16Table[tableIndex]) & 0xFFFF);
        }
    }
    return (crc);
}

bool rbEnd(void)
{
    bool deinitialised = false;
    if(context.serialDeInit())
    {
        deinitialised = true;
        serialState = CLOSED;
    }
    return deinitialised;
}

static bool checkProvisioning(uint16_t topic)
{
    bool provisioned = false;
    int count = 0;

    if(topic >= IMT_MIN_TOPIC_ID && topic <= IMT_MAX_TOPIC_ID)
    {
        if (messageProvisioningInfo.provisioningSet)
        {
            count = messageProvisioningInfo.topicCount;
            if(count > 0)
            {
                for (int i = 0; i < count && i < JSPR_MAX_TOPICS; i++)
                {
                    if(messageProvisioningInfo.provisioning[i].topicId == topic)
                    {
                        provisioned = true;
                    }
                }
            }
        }
        else
        {
            if(jsprGetMessageProvisioning())
            {
                receiveJspr(&response, "messageProvisioning");
                if(JSPR_RC_NO_ERROR == response.code && strcmp(response.target, "messageProvisioning") == 0)
                {
                    jsprMessageProvisioning_t messageProvisioning;
                    if(parseJsprGetMessageProvisioning(response.json, &messageProvisioning))
                    {
                        if(messageProvisioning.provisioningSet)
                        {
                            if(rbCallbacks && rbCallbacks->messageProvisioning)
                            {
                                rbCallbacks->messageProvisioning(&messageProvisioning);
                            }
                        }
                        messageProvisioningInfo = messageProvisioning;
                        count = messageProvisioning.topicCount;
                        if(count > 0)
                        {
                            for (int i = 0; i < count && i < JSPR_MAX_TOPICS; i++)
                            {
                                if(messageProvisioning.provisioning[i].topicId == topic)
                                {
                                    provisioned = true;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return provisioned;
}

#if defined(KERMIT)
#include "kermit_io.h"

struct k_data kermitData;
struct k_response kermitResponse;
int kermitStatus = 0;
unsigned char i_buf[IBUFLEN+8];

bool rbUpdateFirmware (const char * firmwareFile, updateProgressCallback progress, void * context)
{
    const char * firmwareFileList[2] = {firmwareFile, NULL};
    unsigned char *inputBufferPtr = (unsigned char *)0; // E-Kermit doesn't like NULL
    short receiveSlot = 0;
    int kermitRxLength = 0;
    void * contextPtr = context;

    jsprOperationalState_t state;
    jsprFirmwareInfo_t firmware;
    jsprBootInfo_t bootInfo;

    bool kermitDone = false;
    bool isInactive = false;
    bool isInKermitMode = false;
    bool firmwareUpdated = false;

    memset(&kermitData, 0, sizeof(kermitData));
    memset(&kermitResponse, 0, sizeof(kermitResponse));

    const long filesize = kermit_io_filesize(firmwareFile);
    if (filesize <= 0)
    {
        // invalid firmware file
        return firmwareUpdated;
    }

    if(jsprGetOperationalState())
    {
        // Wait for 200 Operational State
        if (waitForJsprMessage(&response, "operationalState", JSPR_RC_NO_ERROR, 1) == true)
        {
            parseJsprGetOperationalState(response.json, &state);
            if (state.operationalState != INACTIVE)
            {
                putOperationalState(INACTIVE);
                // Look for 299 Operational State, this indicates it is actually inactive
                if (waitForJsprMessage(&response, "operationalState", JSPR_RC_UNSOLICITED_MESSAGE, 1) == true)
                {
                    parseJsprGetOperationalState(response.json, &state);
                    isInactive = state.operationalState == INACTIVE;
                }
            }

            if (state.operationalStateSet == true)
            {
                isInactive = state.operationalState == INACTIVE;
            }
        }
    }

    if (isInactive == true)
    {
        if (jsprPutFirmware(JSPR_BOOT_SOURCE_PRIMARY))
        {
            if(receiveJspr(&response, "firmware"))
            {
                if(JSPR_RC_NO_ERROR == response.code)
                {
                    isInKermitMode = parseJsprFirmwareInfo(response.json, &firmware);
                }
            }
        }
    }

    if (isInKermitMode == true)
    {;
        kermit_io_init_string();

        delay(1000);

        kermitData.xfermode = 0;                                         /* Automatic Mode  */
        kermitData.remote = 0;                                           /* Local */
        kermitData.binary = 1;                                           /* Binary */
        kermitData.parity = PAR_NONE;                                    /* No parity */
        kermitData.bct = 1;                                              /* Use Block check type 3 */
        kermitData.ikeep = OFF;                                          /* Don't keep files but pointless i this implementation */
        kermitData.filelist = (unsigned char **)&firmwareFileList;       /* List of files to send (if any) */
        kermitData.cancel = 0;                                           /* Not canceled yet */

        /*  Fill in the i/o pointers  */
        kermitData.zinbuf = i_buf;                                       /* File input buffer */
        kermitData.zinlen = IBUFLEN;                                     /* File input buffer length */
        kermitData.zincnt = 0;                                           /* File input buffer position */
        kermitData.obuf = (unsigned char *)0;                            /* File output buffer */
        kermitData.obuflen = 0;                                          /* File output buffer length */
        kermitData.obufpos = 0;                                          /* File output buffer position */

        kermitData.rxd    = kermit_io_readpkt;
        kermitData.txd    = kermit_io_tx_data;
        kermitData.ixd    = kermit_io_inchk;
        kermitData.openf  = kermit_io_openfile;
        kermitData.finfo  = 0;
        kermitData.readf  = kermit_io_readfile;
        kermitData.writef = 0;
        kermitData.closef = kermit_io_closefile;
        kermitData.dbf    = 0;

        kermitStatus = kermit(K_INIT, &kermitData, 0, 0, 0, &kermitResponse);
        if (kermitStatus == SUCCESS)
        {
            kermitResponse.filesize = filesize; // Need a file size for a progress callback

            kermitStatus = kermit(K_SEND, &kermitData, 0, 0, 0, &kermitResponse);
            if (kermitStatus == SUCCESS)
            {
                while (kermitStatus != X_RC_DONE)
                {
                    inputBufferPtr = (unsigned char *)0; // E-Kermit doesn't like NULL;
                    receiveSlot = -1;
                    kermitRxLength = 0;

                    if (kermitData.ixd(&kermitData) > 0)
                    {
                        inputBufferPtr = getrslot(&kermitData, &receiveSlot);
                        kermitRxLength = kermitData.rxd(&kermitData, inputBufferPtr, P_PKTLEN);

                        if (kermitRxLength < 1)
                        {
                            freesslot(&kermitData, receiveSlot);
                            break;
                        }
                    }

                    kermitStatus = kermit(K_RUN, &kermitData, receiveSlot, kermitRxLength, 0, &kermitResponse);
                    switch (kermitStatus)
                    {
                        case X_RC_OK:
                            if ((kermitResponse.status == S_DATA) && (progress != NULL))
                            {
                                progress(contextPtr, kermitResponse.sofar, kermitResponse.filesize);
                            }
                        break;
                        case X_RC_DONE:
                            kermitDone = true;
                        break;
                        case X_RC_ERROR:
                            // printf("Kermit K_RUN error\n");
                        break;
                    }
                }
            }
        }
    }

    if (kermitDone == true)
    {
        // Since board revision 2 (note that board revision 1 was never publicly available)
        // When kermit is done, the 9704 modem will reboot, as a result the USB
        // serial driver will disabled then re-enable, so we will need to re-enable
        // the library for the new usb port, it might be different.
        firmwareUpdated = true;
    }

    return firmwareUpdated;
}
#endif
