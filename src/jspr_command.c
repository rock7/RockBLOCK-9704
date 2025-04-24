#include "jspr_command.h"
#include "serial.h"
#include <stdlib.h>
#include <string.h>

extern serialContext context;
extern int messageReference;
static uint8_t jsprCommandBuffer [COMMAND_MAX_LEN];

#define JSPR_GET_API_LEN 18U
#define JSPR_GET_SIM_CONFIG_LEN 17U
#define JSPR_GET_OPERATIONAL_STATE_LEN 24U
#define JSPR_GET_SIGNAL_LEN 26U
#define JSPR_GET_MESSAGE_PROVISIONING_LEN 27U
#define JSPR_GET_HW_INFO_LEN 14U
#define JSPR_PUT_FIRMWARE_LEN 17U
#define JSPR_BOOT_SOURCE_STR_LEN 9U
#define JSPR_GET_FIRMWARE_LEN 16U
#define JSPR_GET_SIM_STATUS_LEN 17U

bool jsprGetApiVersion(void)
{
    bool rVal = false;
    const char getApiVersionStr[JSPR_GET_API_LEN] = "GET apiVersion {}\r";
    if (context.serialWrite != NULL)
    {
        if(sendJspr(getApiVersionStr, JSPR_GET_API_LEN) == JSPR_GET_API_LEN)
        {
            rVal = true;
        }
    }
    return rVal;
}

bool jsprPutApiVersion(const jsprDottedVersion_t * apiVersion)
{
    bool rVal = false;
    int rc = 0;
    rc = snprintf(jsprCommandBuffer, sizeof(jsprCommandBuffer),
            "PUT apiVersion {\"active_version\": {\"major\": %d, \"minor\": %d, \"patch\": %d}}\r",
            apiVersion->major, apiVersion->minor, apiVersion->patch);

    if (rc > 0)
    {
        const size_t putApiVersionStrLen = (const size_t)rc;
        if (context.serialWrite != NULL)
        {
            if(sendJspr(jsprCommandBuffer, putApiVersionStrLen) == putApiVersionStrLen)
            {
                rVal = true;
            }
        }
    }

    return rVal;
}

bool putSimInterface(const availableSimInterfaces_t iface)
{
    bool rVal = false;

    switch (iface)
    {
        case NONE:
            rVal =  jsprPutSimInterface("none");
        break;

        case LOCAL:
            rVal =  jsprPutSimInterface("local");
        break;

        case REMOTE:
            rVal =  jsprPutSimInterface("remote");
        break;

        case INTERNAL:
        // Fall through
        default:
            rVal =  jsprPutSimInterface("internal");
        break;
    }

    return rVal;
}

bool jsprGetSimInterface(void)
{
    bool rVal = false;
    const char getSimInterfaceStr[JSPR_GET_SIM_CONFIG_LEN] = "GET simConfig {}\r";
    if (context.serialWrite != NULL)
    {
        if(sendJspr(getSimInterfaceStr, JSPR_GET_SIM_CONFIG_LEN) == JSPR_GET_SIM_CONFIG_LEN)
        {
            rVal = true;
        }
    }
    return rVal;
}

bool jsprPutSimInterface(const char * iface)
{
    bool rVal = false;
    int rc = 0;

    rc = snprintf(jsprCommandBuffer, sizeof(jsprCommandBuffer),
            "PUT simConfig {\"interface\": \"%s\"}\r", iface);

    if (rc > 0)
    {
        const size_t putSimInterfaceStrLen = (const size_t)rc;
        if (context.serialWrite != NULL)
        {
            if(sendJspr(jsprCommandBuffer, putSimInterfaceStrLen) == putSimInterfaceStrLen)
            {
                rVal = true;
            }
        }
    }

    return rVal;
}

bool putOperationalState(availableOperationalStates_t state)
{
    bool rVal = false;

    switch (state)
    {
        case INACTIVE:
            rVal = jsprPutOperationalState("inactive");
        break;

        case CAL_TEST:
            rVal = jsprPutOperationalState("cal_test");
        break;

        case HW_SELF_TEST:
            rVal = jsprPutOperationalState("hw_self_test");
        break;

        case RF_SCAN:
            rVal = jsprPutOperationalState("rf_scan");
        break;

        case LOOPBACK:
            rVal = jsprPutOperationalState("loopback");
        break;

        case FAULT:
            rVal = jsprPutOperationalState("fault");
        break;

        case ACTIVE:
        // fall through
        default:
            rVal = jsprPutOperationalState("active");
        break;
    }

    return rVal;
}

bool jsprGetOperationalState(void)
{
    bool rVal = false;
    const char getOperationalStateStr[JSPR_GET_OPERATIONAL_STATE_LEN] = "GET operationalState {}\r";

    if (context.serialWrite != NULL)
    {
        if(sendJspr(getOperationalStateStr, JSPR_GET_OPERATIONAL_STATE_LEN) == JSPR_GET_OPERATIONAL_STATE_LEN)
        {
            rVal = true;
        }
    }
    return rVal;
}

bool jsprPutOperationalState(const char * state)
{
    bool rVal = false;
    int rc = 0;

    rc = snprintf(jsprCommandBuffer, sizeof(jsprCommandBuffer),
            "PUT operationalState {\"state\": \"%s\"}\r", state);

    if (rc > 0)
    {
        const size_t putOperationalStateStrLen = (const size_t)rc;
        if (context.serialWrite != NULL)
        {
            if(sendJspr(jsprCommandBuffer, putOperationalStateStrLen) == putOperationalStateStrLen)
            {
                rVal = true;
            }
        }
    }

    return rVal;
}

bool jsprPutMessageOriginate(const uint16_t topic, const size_t length)
{
    bool rVal = false;
    int rc = 0;

    rc = snprintf(jsprCommandBuffer, sizeof(jsprCommandBuffer),
            "PUT messageOriginate {\"topic_id\":%d, \"message_length\":%ld, \"request_reference\":%d}\r",
            topic, length, messageReference);

    if (rc > 0)
    {
        messageReference++;
        const size_t putMessageOriginateStrLen = (const size_t)rc;
        if (context.serialWrite != NULL)
        {
            if(sendJspr(jsprCommandBuffer, putMessageOriginateStrLen) == putMessageOriginateStrLen)
            {
                rVal = true;
            }
        }
    }

    return rVal;
}

bool jsprPutMessageOriginateSegment(jsprMessageOriginate_t * messageOriginate, const size_t segmentLength, uint32_t segmentStart, const char * data)
{
    bool rVal = false;
    int rc = 0;

    rc = snprintf(jsprCommandBuffer, sizeof(jsprCommandBuffer),
            "PUT messageOriginateSegment {\"topic_id\":%d, \"message_id\":%d, \"segment_length\":%ld, \"segment_start\":%d, \"data\":\"%s\"}\r",
            messageOriginate->topic, messageOriginate->messageId, segmentLength, segmentStart, data);

    if (rc > 0)
    {
        const size_t putMessageOriginateSegmentStrLen = (const size_t)rc;
        if (context.serialWrite != NULL)
        {
            if(sendJspr(jsprCommandBuffer, putMessageOriginateSegmentStrLen) == putMessageOriginateSegmentStrLen)
            {
                rVal = true;
            }
        }
    }

    return rVal;
}

bool jsprGetSignal(void)
{
    bool rVal = false;
    const char getSignalStr[JSPR_GET_SIGNAL_LEN] = "GET constellationState {}\r";

    if (context.serialWrite != NULL)
    {
        if(sendJspr(getSignalStr, JSPR_GET_SIGNAL_LEN) == JSPR_GET_SIGNAL_LEN)
        {
            rVal = true;
        }
    }
    return rVal;
}

bool jsprGetMessageProvisioning(void)
{
    bool rVal = false;
    const char getMessageProvisioningStr[JSPR_GET_MESSAGE_PROVISIONING_LEN] = "GET messageProvisioning {}\r";
    if (context.serialWrite != NULL)
    {
        if(sendJspr(getMessageProvisioningStr, JSPR_GET_MESSAGE_PROVISIONING_LEN) == JSPR_GET_MESSAGE_PROVISIONING_LEN)
        {
            rVal = true;
        }
    }
    return rVal;
}

bool jsprGetHwInfo(void)
{
    bool rVal = false;
    const char getHwInfoStr[JSPR_GET_HW_INFO_LEN] = "GET hwInfo {}\r";
    if (context.serialWrite != NULL)
    {
        if(sendJspr(getHwInfoStr, JSPR_GET_HW_INFO_LEN) == JSPR_GET_HW_INFO_LEN)
        {
            rVal = true;
        }
    }
    return rVal;
}

static void bootSlotToStr(const jsprBootSource_t slot, char * dest, const size_t length)
{
    if (length > 0)
    {
        switch(slot)
        {
            case JSPR_BOOT_SOURCE_FALLBACK:
                strncpy(dest, "fallback", length - 1);
            break;

            case JSPR_BOOT_SOURCE_UNKNOWN:
            // fall through
            case JSPR_BOOT_SOURCE_PRIMARY:
            // fall through
            default:
                strncpy(dest, "primary", length - 1);
        }
    }
}

bool jsprGetFirmware(const jsprBootSource_t slot)
{
    bool rVal = false;
    int rc = 0;
    char slotStr [JSPR_BOOT_SOURCE_STR_LEN];

    bootSlotToStr(slot, slotStr, JSPR_BOOT_SOURCE_STR_LEN);

    rc = snprintf(jsprCommandBuffer, sizeof(jsprCommandBuffer), "GET firmware {\"slot\": \"%s\"}\r", slotStr);

    if (rc > 0)
    {
        const size_t getFirmwareStrLen = (const size_t)rc;
        if (context.serialWrite != NULL)
        {
            if(sendJspr(jsprCommandBuffer, getFirmwareStrLen) == getFirmwareStrLen)
            {
                rVal = true;
            }
        }
    }

    return rVal;
}

bool jsprPutFirmware(const jsprBootSource_t slot)
{
    bool rVal = false;
    int rc = 0;
    char slotStr [JSPR_BOOT_SOURCE_STR_LEN];

    bootSlotToStr(slot, slotStr, JSPR_BOOT_SOURCE_STR_LEN);

    rc = snprintf(jsprCommandBuffer, sizeof(jsprCommandBuffer), "PUT firmware {\"slot\": \"%s\"}\r", slotStr);

    if (rc > 0)
    {
        const size_t putFirmwareStrLen = (const size_t)rc;
        if (context.serialWrite != NULL)
        {
            if(sendJspr(jsprCommandBuffer, putFirmwareStrLen) == putFirmwareStrLen)
            {
                rVal = true;
            }
        }
    }

    return rVal;
}

bool jsprGetSimStatus(void)
{
    bool rVal = false;
    const char getSimStatusStr[JSPR_GET_SIM_STATUS_LEN] = "GET simStatus {}\r";
    if (context.serialWrite != NULL)
    {
        if(sendJspr(getSimStatusStr, JSPR_GET_SIM_STATUS_LEN) == JSPR_GET_SIM_STATUS_LEN)
        {
            rVal = true;
        }
    }
    return rVal;
}