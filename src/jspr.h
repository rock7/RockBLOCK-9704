#ifndef JSPR_H
#define JSPR_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "crossplatform.h"

#define SKIPPED_UNSOLICITED 5U
#define RX_BUFFER_SIZE 8192U
#define TX_BUFFER_SIZE 8192U

#define JSPR_MAX_TARGET_LENGTH 30U
#define JSPR_RESULT_CODE_LENGTH 3U
#define JSPR_MIN_RESPONSE 9U
#define JSPR_MAX_TARGET_LENGTH 30U
#define JSPR_MAX_JSON_LENGTH 2000U
#define JSPR_MAX_SEGMENT_LENGTH 1447U
#define JSPR_MAX_NUM_API_VERSIONS 2U
#define JSPR_VERSION_INFO_BUILD_INFO_LEN 50U
#define JSPR_BOOT_INFO_IMAGE_TYPE_LEN 11U
#define JSPR_BOOT_INFO_HASH_LEN 65U

#define JSPR_TOPIC_NAME_MAX_LENGTH 50U
#define JSPR_MAX_TOPICS 10U

#define JSPR_HW_VERSION_MAX_LENGTH 7U
#define JSPR_SERIAL_NUMBER_MAX_LENGTH 7U
#define JSPR_IMEI_MAX_LENGTH 16U

#define JSPR_ICCID_MAX_LENGTH 20U

enum responseCodes
{
    JSPR_RC_NO_ERROR = 200, 
    JSPR_RC_UNSOLICITED_MESSAGE = 299, 
    JSPR_RC_API_VERSION_NOT_SELECTED = 400, 
    JSPR_RC_UNSUPPORTED_REQUEST_TYPE = 401,
    JSPR_RC_CONFIGURATION_ALREADY_SET = 402, 
    JSPR_RC_COMMAND_TOO_LONG = 403, 
    JSPR_RC_UNKNOWN_TARGET = 404, 
    JSPR_RC_COMMAND_MALFORMED = 405, 
    JSPR_RC_OPERATION_NOT_ALLOWED = 406, 
    JSPR_RC_BAD_JSON = 407, 
    JSPR_RC_REQUEST_FAILED = 408, 
    JSPR_RC_UNAUTHORIZED = 409, 
    JSPR_RC_SIM_NOT_CONFIGURED = 410, 
    JSPR_RC_WAKE_XCVR_IN_INVALID = 411, 
    JSPR_RC_INVALID_CHANNEL = 412, 
    JSPR_RC_INVALID_ACTION = 413, 
    JSPR_RC_HARDWARE_NOT_CONFIGURED = 414, 
    JSPR_RC_INVALID_RADIO_PATH = 415, 
    JSPR_RC_CRASH_DUMP_NOT_AVAILABLE = 416, 
    JSPR_RC_FEATURE_NOT_SUPPORTED_BY_HARDWARE = 417, 
    JSPR_RC_NOT_PROVISIONED = 418, 
    JSPR_RC_INVALID_TRANSMIT_POWER = 419, 
    JSPR_RC_INVALID_BURST_TYPE = 420, 
    JSPR_RC_SERIAL_PORT_ERROR = 500
};

typedef struct
{
    uint32_t code;
    char target[JSPR_MAX_TARGET_LENGTH];
    char json[JSPR_MAX_JSON_LENGTH];
    uint16_t jsonSize;
} jsprResponse_t;

typedef struct
{
    uint8_t major;
    uint8_t minor;
    uint8_t patch;
} jsprDottedVersion_t;

typedef struct
{
    jsprDottedVersion_t version;
    char buildInfo [JSPR_VERSION_INFO_BUILD_INFO_LEN];
} jsprVersionInfo_t;

typedef enum
{
    JSPR_BOOT_SOURCE_UNKNOWN,
    JSPR_BOOT_SOURCE_PRIMARY,
    JSPR_BOOT_SOURCE_FALLBACK
} jsprBootSource_t;

typedef struct
{
    char imageType[JSPR_BOOT_INFO_IMAGE_TYPE_LEN];
    jsprBootSource_t bootSource;
    jsprVersionInfo_t versionInfo;
} jsprBootInfo_t;

typedef struct
{
    jsprDottedVersion_t supportedVersions[JSPR_MAX_NUM_API_VERSIONS];
    uint8_t             supportedVersionCount;
    bool                activeVersionSet;
    jsprDottedVersion_t activeVersion;
} jsprApiVersion_t;

typedef enum
{
    NONE,
    LOCAL,
    REMOTE,
    INTERNAL
}availableSimInterfaces_t;

typedef struct
{
    bool ifaceSet;
    availableSimInterfaces_t iface;
} jsprSimInterface_t;

typedef struct
{
    jsprBootSource_t slot;
    bool validity;
    jsprVersionInfo_t versionInfo;
    char hash[JSPR_BOOT_INFO_HASH_LEN];
} jsprFirmwareInfo_t;

typedef enum
{
    INACTIVE,
    ACTIVE,
    CAL_TEST,
    HW_SELF_TEST,
    RF_SCAN,
    LOOPBACK,
    FAULT
}availableOperationalStates_t;

typedef enum
{
    NORMAL,
    HARDWARE_SELF_TEST_FAILURE,
    TEMPERATURE_FAULT,
    RF_POWER_PROTECTION_FAULT,
    VAM_APP_FAILED_ACK_INTERFACE_TRANSITION_FLOWING,
    INVALID_HARDWARE,
    LOW_SUPPLY_VOLTAGE,
    MFRTEST_USED_INCORRECTLY
}operationalStateReason_t;

typedef struct
{
    bool operationalStateSet;
    operationalStateReason_t reason;
    availableOperationalStates_t operationalState;
}jsprOperationalState_t;

typedef enum
{
    MESSAGE_ACCEPTED,
    SUBSCRIPTION_INVALID,
    MESSAGE_DISCARDED_ON_OVERFLOW
}messageOriginateResponses_t;

typedef struct
{
    uint16_t topic;
    uint8_t requestReference;
    uint8_t messageId;
    bool messageIdSet;
    messageOriginateResponses_t messageResponse;
} jsprMessageOriginate_t;

typedef struct
{
    uint16_t topic;
    uint8_t messageId;
    uint16_t segmentLength;
    uint32_t segmentStart;
} jsprMessageOriginateSegment_t;

typedef struct
{
    uint16_t topic;
    uint8_t messageId;
    uint32_t messageLengthMax;
} jsprMessageTerminate_t;

typedef struct
{
    uint16_t topic;
    uint8_t messageId;
    uint16_t segmentLength;
    uint32_t segmentStart;
    char data[JSPR_MAX_SEGMENT_LENGTH];
    size_t dataLength;
} jsprMessageTerminateSegment_t;

typedef struct
{
    bool constellationVisible;
    uint8_t signalBars;
    int16_t signalLevel;
} jsprConstellationState_t;

typedef enum
{
    MO_ACK_RECEIVED_MOS,
    MESSAGE_DISCARDED_ON_OVERFLOW_MOS,
    MESSAGE_EXPIRED_MOS,
    MESSAGE_TRANSFER_TIMEOUT_MOS,
    SEGMENT_NOT_SUPPLIED_MOS,
    SEGMENT_INCORRECT_MOS,
    NETWORK_ERROR_MOS,
    MESSAGE_CANCELLED_PRE_TRANSIT_MOS,
    MESSAGE_CANCELLED_IN_TRANSIT_MOS,
    SUBSCRIPTION_INVALID_MOS,
    PROTOCOL_ERROR_MOS,
    MESSAGE_DROPPED_LOCAL_CRC_ERROR_MOS,
    CRC_ERROR_IN_TRANSFER_MOS,
    USER_SUPPLIED_CRC_ERROR_MOS
}jsprFinalMoStatus_t;

typedef struct
{
    uint16_t topic;
    uint8_t messageId;
    jsprFinalMoStatus_t finalMoStatus;
} jsprMessageOriginateStatus_t;

typedef enum
{
    COMPLETE,
    MESSAGE_TIMED_OUT,
    MESSAGE_CANCELLED,
    CRC_ERROR_IN_TRANSFER,
}jsprFinalMtStatus_t;

typedef struct
{
    uint16_t topic;
    uint8_t messageId;
    jsprFinalMtStatus_t finalMtStatus;
} jsprMessageTerminateStatus_t;

typedef enum
{
    SAFETY_1,
    SAFETY_2,
    SAFETY_3,
    HIGH_PRIORITY,
    MEDIUM_PRIORITY,
    LOW_PRIORITY
}jsprTopicPriority_t;

typedef struct
{
    uint16_t topicId;
    char topicName[JSPR_TOPIC_NAME_MAX_LENGTH];
    jsprTopicPriority_t priority;
    uint32_t discardTimeSeconds;
    uint8_t maxQueueDepth;
} jsprProvisioning_t;

typedef struct
{
    jsprProvisioning_t provisioning[JSPR_MAX_TOPICS];
    uint8_t topicCount;
} jsprMessageProvisioning_t;

typedef struct
{
    char hwVersion[JSPR_HW_VERSION_MAX_LENGTH];
    char serialNumber[JSPR_SERIAL_NUMBER_MAX_LENGTH];
    char imei[JSPR_IMEI_MAX_LENGTH];
    //uint8_t pa_temp; not supported by 9704?
    int8_t boardTemp;
} jsprHwInfo_t;

typedef struct
{
    bool cardPresent;
    bool simConnected;
    char iccid[JSPR_ICCID_MAX_LENGTH];
} jsprSimStatus_t;

//internal functions
int sendJspr(const char * buffer, size_t length);
bool receiveJspr(jsprResponse_t * response, bool skipUnsolicited);
void clearResponse(jsprResponse_t * response);
bool parseJsprBootInfo(const char * jsprString, jsprBootInfo_t * bootInfo);
bool parseJsprGetApiVersion(char * jsprString, jsprApiVersion_t * apiVersion);
bool parseJsprFirmwareInfo(const char * jsprString, jsprFirmwareInfo_t * firmwareInfo);
bool parseJsprGetSimInterface(char * jsprString, jsprSimInterface_t * simInterface);
bool parseJsprGetOperationalState(char * jsprString, jsprOperationalState_t * operationalState);
bool parseJsprPutMessageOriginate(char * jsprString, jsprMessageOriginate_t * messageOriginate);
bool parseJsprUnsMessageOriginateSegment(char * jsprString, jsprMessageOriginateSegment_t * messageOriginateSegment);
bool parseJsprUnsMessageTerminate(char * jsprString, jsprMessageTerminate_t * messageTerminate);
bool parseJsprUnsMessageTerminateSegment(char * jsprString, jsprMessageTerminateSegment_t * messageTerminateSegment);
bool parseJsprGetSignal(char * jsprString, jsprConstellationState_t * signal);
bool parseJsprUnsMessageOriginateStatus(char * jsprString, jsprMessageOriginateStatus_t * messageOriginateStatus);
bool parseJsprUnsMessageTerminateStatus(char * jsprString, jsprMessageTerminateStatus_t * messageTerminateStatus);
bool parseJsprGetMessageProvisioning(char * jsprString, jsprMessageProvisioning_t * messageProvisioning);
bool parseJsprGetHwInfo(char * jsprString, jsprHwInfo_t * hwInfo);
bool parseJsprGetSimStatus(char * jsprString, jsprSimStatus_t * simStatus);

#ifdef __cplusplus
}
#endif

#endif