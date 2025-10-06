#include "jspr.h"
#include "jspr_command.h"
#include "serial.h"
#include "third_party/cJSON/cJSON.h"
#include <string.h>
#include <stdlib.h>
#include "crossplatform.h"
#if defined(_WIN32)
#include <io.h>
#define access _access
#else
#include <unistd.h>
#endif

//Messaging Variables
int messageReference = 1;
static uint8_t jsprRxBuffer [RX_BUFFER_SIZE];
extern serialContext context;

int sendJspr(const char *buffer, size_t length)
{
        int bytesWritten = context.serialWrite(buffer, length);
        if(0 > bytesWritten)
        {
            return -1;
        }
#ifdef DEBUG
        char * terminator = strpbrk(buffer, "\r");
        *terminator = '\0';
        printf("SENT: %s\r\n", buffer);
#endif
        return bytesWritten;
}

bool receiveJspr(jsprResponse_t * response, const char * expectedTarget)
{
    bool received = false;
    int loop;
    char resultCode[JSPR_RESULT_CODE_LENGTH + 1]; // Plus 1 for the NULL
    uint16_t pos = 0;
    int bytesRead;
    bool validResponse = false;
    bool reading = true;
    bool gotResponse = false;
    char * targetStart = NULL;
    char * targetEnd = NULL;
    uint16_t targetLength = 0;
    size_t resultCodeIndexStart = 0;
    char * jsonStart = NULL;

    clearResponse(response); //make sure we're dealing with an empty structure
    if((context.serialRead != NULL) && (response != NULL))
    {
        memset(jsprRxBuffer, 0 , RX_BUFFER_SIZE);
        do
        {
            while(pos < (RX_BUFFER_SIZE - 1))
            {
                bytesRead = context.serialRead(&jsprRxBuffer[pos], 1);
                if (bytesRead <= 0)
                {
                    reading = false; //make function non-blocking
                    break;
                }
                if (jsprRxBuffer[pos] == '\r' && pos > 2)
                {
                    jsprRxBuffer[pos] = '\0'; // Replace with NULL
                    validResponse = true;
                    break;
                }
                pos++;
            }

            if(validResponse == true)
            {
#ifdef DEBUG
            printf("RECEIVED: %s\r\n", jsprRxBuffer);
#endif
                if (pos >= JSPR_MIN_RESPONSE)
                {
                    // Strip unwanted characters at the start, this can happen with bootInfo message
                    // this seems to be DC1 character at the start
                    while ((response->code < JSPR_RC_NO_ERROR) || (response->code > JSPR_RC_SERIAL_PORT_ERROR))
                    {
                        if ((RX_BUFFER_SIZE - resultCodeIndexStart) <  JSPR_RESULT_CODE_LENGTH)
                        {
                            break;
                        }

                        for(uint8_t i = 0; i < JSPR_RESULT_CODE_LENGTH; i++)
                        {
                            resultCode[i] = jsprRxBuffer[i + resultCodeIndexStart];
                        }

                        resultCode[JSPR_RESULT_CODE_LENGTH] = '\0';
                        response->code = (uint16_t)atoi(resultCode);

                        if ((response->code < JSPR_RC_NO_ERROR) || (response->code > JSPR_RC_SERIAL_PORT_ERROR))
                        {
                            resultCodeIndexStart++;
                        }
                    };

                    if (resultCodeIndexStart > 0)
                    {
                        memmove(jsprRxBuffer, &jsprRxBuffer[resultCodeIndexStart], (pos - resultCodeIndexStart));
                    }

                    targetStart = &jsprRxBuffer[JSPR_RESULT_CODE_LENGTH + 1];
                    targetEnd = strchr(targetStart, ' ');
                    targetLength = targetEnd - targetStart;
                    memcpy(response->target, targetStart, targetLength);
                    response->target[targetLength] = '\0';

                    if (expectedTarget != NULL)
                    {
                        if (strncmp(response->target, expectedTarget, JSPR_MAX_TARGET_LENGTH) !=0)
                        {
                            pos = 0;
                            memset(jsprRxBuffer, 0 , RX_BUFFER_SIZE);
                            memset(response, 0, sizeof(response));
                            continue;
                        }
                    }

                    jsonStart = strchr(targetStart, '{');
                    response->jsonSize = strchr(targetStart, '\0') - jsonStart;
                    strncpy(response->json, jsonStart, response->jsonSize);
                    response->json[response->jsonSize] = '\0';
                    reading = false;
                    gotResponse = true;
                    received = true;
                }
            }
        }while(reading == true);
    }
    return received;
}

bool waitForJsprMessage(jsprResponse_t * response, const char * expectedTarget, const uint32_t expectedCode, const uint32_t timeoutSeconds)
{
    bool gotMessage = false;
    unsigned long startTime = millis();

    while (1)
    {
        receiveJspr(response, expectedTarget);

        if (response->code == expectedCode &&
            strncmp(response->target, expectedTarget, JSPR_MAX_TARGET_LENGTH) == 0)
        {
            gotMessage = true;
            break;
        }

        if ((millis() - startTime) > timeoutSeconds * 1000)
        {
            gotMessage = false;
            break;
        }

        delay(10);
    }

    return gotMessage;
}

void clearResponse(jsprResponse_t * response)
{
    response->code = 0;
    response->jsonSize = 0;
    memset(response->json, 0, JSPR_MAX_JSON_LENGTH);
    memset(response->target, 0, JSPR_MAX_TARGET_LENGTH);
}

bool parseJsprBootInfo(const char * jsprString, jsprBootInfo_t * bootInfo)
{
    bool parsed = false;
    cJSON * json = NULL;
    cJSON * imageType = NULL;
    cJSON * bootSource = NULL;
    cJSON * version = NULL;
    cJSON * major =  NULL;
    cJSON * minor =  NULL;
    cJSON * patch =  NULL;
    cJSON * buildInfo =  NULL;
    char * cPtr = NULL;

    if ((jsprString != NULL) && (bootInfo != NULL))
    {
        json = cJSON_Parse(jsprString);

        if (json != NULL)
        {
            imageType = cJSON_GetObjectItem(json, "image_type");
            bootSource = cJSON_GetObjectItem(json, "boot_source");
            version = cJSON_GetObjectItem(json, "version");
        }

        if (imageType != NULL)
        {
            cPtr = stpncpy(bootInfo->imageType, imageType->valuestring, JSPR_BOOT_INFO_IMAGE_TYPE_LEN - 1);
            *cPtr = '\0';
            cPtr = NULL;
        }

        if (bootSource != NULL)
        {
            if (strcmp(bootSource->valuestring, "primary") == 0)
            {
                bootInfo->bootSource = JSPR_BOOT_SOURCE_PRIMARY;
            }
            else if (strcmp(bootSource->valuestring, "fallback") == 0)
            {
                bootInfo->bootSource = JSPR_BOOT_SOURCE_FALLBACK;
            }
            else
            {
                bootInfo->bootSource = JSPR_BOOT_SOURCE_UNKNOWN;
            }
        }

        if (version != NULL)
        {
            major = cJSON_GetObjectItem(version, "major");
            minor = cJSON_GetObjectItem(version, "minor");
            patch = cJSON_GetObjectItem(version, "patch");
            buildInfo = cJSON_GetObjectItem(version, "build_info");

            if (major != NULL)
            {
                bootInfo->versionInfo.version.major = major->valueint;
            }

            if (minor != NULL)
            {
                bootInfo->versionInfo.version.minor = minor->valueint;
            }

            if (patch != NULL)
            {
                bootInfo->versionInfo.version.patch = patch->valueint;
            }

            if (buildInfo != NULL)
            {
                cPtr = stpncpy(bootInfo->versionInfo.buildInfo, buildInfo->valuestring, JSPR_VERSION_INFO_BUILD_INFO_LEN - 1);
                *cPtr = '\0';
                cPtr = NULL;
            }
        }
        cJSON_Delete(json);
        parsed = true;
    }

    return parsed;
}

bool parseJsprGetApiVersion(char * jsprString, jsprApiVersion_t * apiVersion)
{
    bool parsed = false;

    if ((jsprString != NULL) && (apiVersion != NULL))
    {
        cJSON * root = cJSON_Parse(jsprString);
        if (root != NULL)
        {
            cJSON * supportedVersions = cJSON_GetObjectItem(root, "supported_versions");
            if (cJSON_IsArray(supportedVersions))
            {
                int count = cJSON_GetArraySize(supportedVersions);
                int apiIndex = (count > 0) ? (count - 1) : 0;
                for (int i = 0; i < count && i < JSPR_MAX_NUM_API_VERSIONS; i++)
                {
                    cJSON *version = cJSON_GetArrayItem(supportedVersions, apiIndex);
                    if (cJSON_IsObject(version))
                    {
                        cJSON *major = cJSON_GetObjectItem(version, "major");
                        cJSON *minor = cJSON_GetObjectItem(version, "minor");
                        cJSON *patch = cJSON_GetObjectItem(version, "patch");
                        if (cJSON_IsNumber(major) && cJSON_IsNumber(minor) && cJSON_IsNumber(patch))
                        {
                            apiVersion->supportedVersions[i].major = (uint8_t)major->valueint;
                            apiVersion->supportedVersions[i].minor = (uint8_t)minor->valueint;
                            apiVersion->supportedVersions[i].patch = (uint8_t)patch->valueint;
                            apiVersion->supportedVersionCount++;
                        }
                    }
                    apiIndex--;
                }
            }
            cJSON *activeVersion = cJSON_GetObjectItem(root, "active_version");
            if (cJSON_IsObject(activeVersion))
            {
                cJSON *major = cJSON_GetObjectItem(activeVersion, "major");
                cJSON *minor = cJSON_GetObjectItem(activeVersion, "minor");
                cJSON *patch = cJSON_GetObjectItem(activeVersion, "patch");
                if (cJSON_IsNumber(major) && cJSON_IsNumber(minor) && cJSON_IsNumber(patch))
                {
                    apiVersion->activeVersion.major = (uint8_t)major->valueint;
                    apiVersion->activeVersion.minor = (uint8_t)minor->valueint;
                    apiVersion->activeVersion.patch = (uint8_t)patch->valueint;
                    apiVersion->activeVersionSet = true;
                }
            }
            else
            {
                apiVersion->activeVersionSet = false;
            }
            parsed = true;
            cJSON_Delete(root);
        }
    }

    return parsed;
}

bool parseJsprFirmwareInfo(const char * jsprString, jsprFirmwareInfo_t * firmwareInfo)
{
    bool parsed = false;
    cJSON * json = NULL;
    cJSON * slot = NULL;
    cJSON * validity = NULL;
    cJSON * version = NULL;
    cJSON * hash = NULL;
    cJSON * major =  NULL;
    cJSON * minor =  NULL;
    cJSON * patch =  NULL;
    cJSON * buildInfo = NULL;
    char * cPtr = NULL;

    if ((jsprString != NULL) && (firmwareInfo != NULL))
    {
        cJSON *json = cJSON_Parse(jsprString);
        if (json != NULL)
        {
            slot = cJSON_GetObjectItem(json, "slot");
            validity = cJSON_GetObjectItem(json, "validity");
            version = cJSON_GetObjectItem(json, "version");
            hash = cJSON_GetObjectItem(json, "hash");

            if (slot != NULL)
            {
                if (strcmp(slot->valuestring, "primary") == 0)
                {
                    firmwareInfo->slot = JSPR_BOOT_SOURCE_PRIMARY;
                }
                else if (strcmp(slot->valuestring, "fallback") == 0)
                {
                    firmwareInfo->slot = JSPR_BOOT_SOURCE_FALLBACK;
                }
                else
                {
                    firmwareInfo->slot = JSPR_BOOT_SOURCE_UNKNOWN;
                }
            }

            if (validity != NULL)
            {
                firmwareInfo->validity = (validity->valueint > 0);
            }

            if (version != NULL)
            {
                major = cJSON_GetObjectItem(version, "major");
                minor = cJSON_GetObjectItem(version, "minor");
                patch = cJSON_GetObjectItem(version, "patch");
                buildInfo = cJSON_GetObjectItem(version, "build_info");

                if (major != NULL)
                {
                    firmwareInfo->versionInfo.version.major = major->valueint;
                }

                if (minor != NULL)
                {
                    firmwareInfo->versionInfo.version.minor = minor->valueint;
                }

                if (patch != NULL)
                {
                    firmwareInfo->versionInfo.version.patch = patch->valueint;
                }

                if (buildInfo != NULL)
                {
                    cPtr = stpncpy(firmwareInfo->versionInfo.buildInfo, buildInfo->valuestring, JSPR_VERSION_INFO_BUILD_INFO_LEN - 1);
                    *cPtr = '\0';
                    cPtr = NULL;
                }
            }

            if (hash != NULL)
            {
                cPtr = stpncpy(firmwareInfo->hash, hash->valuestring, JSPR_BOOT_INFO_HASH_LEN - 1);
                *cPtr = '\0';
                cPtr = NULL;
            }

            parsed = true;
            cJSON_Delete(json);
        }
    }

    return parsed;
}

bool parseJsprGetSimInterface(char * jsprString, jsprSimInterface_t * simInterface)
{
    bool parsed = false;

    if ((jsprString != NULL) && (simInterface != NULL))
    {
        cJSON * root = cJSON_Parse(jsprString);
        if (root != NULL)
        {
            cJSON * iface = cJSON_GetObjectItem(root, "interface");
            if(cJSON_IsString(iface))
            {
                if(strcmp(iface->valuestring, "none") == 0)
                {
                    simInterface->ifaceSet = true;
                    simInterface->iface = SIM_NONE;
                }
                else if (strcmp(iface->valuestring, "local") == 0)
                {
                    simInterface->ifaceSet = true;
                    simInterface->iface = SIM_LOCAL;
                }
                else if (strcmp(iface->valuestring, "remote") == 0)
                {
                    simInterface->ifaceSet = true;
                    simInterface->iface = SIM_REMOTE;
                }
                else if (strcmp(iface->valuestring, "internal") == 0)
                {
                    simInterface->ifaceSet = true;
                    simInterface->iface = SIM_INTERNAL;
                }
            }
            else
            {
                simInterface->ifaceSet = false;
            }
        parsed = true;
        cJSON_Delete(root);
        }
    }

    return parsed;
}

bool parseJsprGetOperationalState(char * jsprString, jsprOperationalState_t * operationalState)
{
    bool parsed = false;

    if ((jsprString != NULL) && (operationalState != NULL))
    {
        cJSON * root = cJSON_Parse(jsprString);
        if (root != NULL)
        {
            cJSON * reason = cJSON_GetObjectItem(root, "reason");
            if(cJSON_IsNumber(reason))
            {
                if(reason->valueint >= NORMAL && reason->valueint <= MFRTEST_USED_INCORRECTLY)
                {
                    operationalState->reason = reason->valueint;
                }
            }
            cJSON * state = cJSON_GetObjectItem(root, "state");
            if(cJSON_IsString(state))
            {
                if(strcmp(state->valuestring, "inactive") == 0)
                {
                    operationalState->operationalStateSet = true;
                    operationalState->operationalState = INACTIVE;
                }
                else if (strcmp(state->valuestring, "active") == 0)
                {
                    operationalState->operationalStateSet = true;
                    operationalState->operationalState = ACTIVE;
                }
                else if (strcmp(state->valuestring, "cal_test") == 0)
                {
                    operationalState->operationalStateSet = true;
                    operationalState->operationalState = CAL_TEST;
                }
                else if (strcmp(state->valuestring, "hw_self_test") == 0)
                {
                    operationalState->operationalStateSet = true;
                    operationalState->operationalState = HW_SELF_TEST;
                }
                else if (strcmp(state->valuestring, "rf_scan") == 0)
                {
                    operationalState->operationalStateSet = true;
                    operationalState->operationalState = RF_SCAN;
                }
                else if (strcmp(state->valuestring, "loopback") == 0)
                {
                    operationalState->operationalStateSet = true;
                    operationalState->operationalState = LOOPBACK;
                }
                else if (strcmp(state->valuestring, "fault") == 0)
                {
                    operationalState->operationalStateSet = true;
                    operationalState->operationalState = FAULT;
                }
            }
            else
            {
                operationalState->operationalStateSet = false;
            }
        parsed = true;
        cJSON_Delete(root);
        }
    }

    return parsed;
}

bool parseJsprPutMessageOriginate(char * jsprString, jsprMessageOriginate_t  * messageOriginate)
{
    bool parsed = false;

    if ((jsprString != NULL) && (messageOriginate != NULL))
    {
        cJSON * root = cJSON_Parse(jsprString);
        if (root != NULL)
        {
            cJSON * topicId = cJSON_GetObjectItem(root, "topic_id");
            if(cJSON_IsNumber(topicId))
            {
                if(topicId->valueint >= 64 && topicId->valueint <= 65535)
                {
                    messageOriginate->topic = topicId->valueint;
                }
            }
            cJSON * requestReference = cJSON_GetObjectItem(root, "request_reference");
            if(cJSON_IsNumber(requestReference))
            {
                if(requestReference->valueint >= 1 && requestReference->valueint <= 100)
                {
                    messageOriginate->requestReference = requestReference->valueint;
                }
            }
            cJSON * messageResponce = cJSON_GetObjectItem(root, "message_response");
            if(cJSON_IsString(messageResponce))
            {
                if(strcmp(messageResponce->valuestring, "message_accepted") == 0)
                {
                    messageOriginate->messageResponse = MESSAGE_ACCEPTED;
                }
                else if (strcmp(messageResponce->valuestring, "subscription_invalid") == 0)
                {
                    messageOriginate->messageResponse = SUBSCRIPTION_INVALID;
                }
                else if (strcmp(messageResponce->valuestring, "message_discarded_on_overflow") == 0)
                {
                    messageOriginate->messageResponse = MESSAGE_DISCARDED_ON_OVERFLOW;
                }
            }
            messageOriginate->messageIdSet = false;
            if(messageOriginate->messageResponse == MESSAGE_ACCEPTED)
            {
                cJSON * messageId = cJSON_GetObjectItem(root, "message_id");
                if(cJSON_IsNumber(messageId))
                {
                    if(messageId->valueint >= 0 && messageId->valueint <= 255)
                    {
                        messageOriginate->messageId = messageId->valueint;
                        messageOriginate->messageIdSet = true;
                    }
                }
            }
        parsed = true;
        cJSON_Delete(root);
        }
    }
    return parsed;
}

bool parseJsprUnsMessageOriginateSegment(char * jsprString, jsprMessageOriginateSegment_t * messageOriginateSegment)
{
    bool parsed = false;

    if ((jsprString != NULL) && (messageOriginateSegment != NULL))
    {
        cJSON * root = cJSON_Parse(jsprString);
        if (root != NULL)
        {
            cJSON * topicId = cJSON_GetObjectItem(root, "topic_id");
            if(cJSON_IsNumber(topicId))
            {
                if(topicId->valueint >= 64 && topicId->valueint <= 65535)
                {
                    messageOriginateSegment->topic = topicId->valueint;
                }
            }
            cJSON * segmentLength = cJSON_GetObjectItem(root, "segment_length");
            if(cJSON_IsNumber(segmentLength))
            {
                if(segmentLength->valueint >= 1 && segmentLength->valueint <= 1446)
                {
                    messageOriginateSegment->segmentLength = segmentLength->valueint;
                }
            }
            cJSON * segmentStart = cJSON_GetObjectItem(root, "segment_start");
            if(cJSON_IsNumber(segmentStart))
            {
                if(segmentStart->valueint >= 0 && segmentStart->valueint <= 100001)
                {
                    messageOriginateSegment->segmentStart = segmentStart->valueint;
                }
            }
            cJSON * messageId = cJSON_GetObjectItem(root, "message_id");
            if(cJSON_IsNumber(messageId))
            {
                if(messageId->valueint >= 0 && messageId->valueint <= 255)
                {
                    messageOriginateSegment->messageId = messageId->valueint;
                }
            }
        parsed = true;
        cJSON_Delete(root);
        }
    }
    return parsed;
}

bool parseJsprUnsMessageTerminate(char * jsprString, jsprMessageTerminate_t * messageTerminate)
{
    bool parsed = false;

    if ((jsprString != NULL) && (messageTerminate != NULL))
    {
        cJSON * root = cJSON_Parse(jsprString);
        if (root != NULL)
        {
            cJSON * topicId = cJSON_GetObjectItem(root, "topic_id");
            if(cJSON_IsNumber(topicId))
            {
                if(topicId->valueint >= 64 && topicId->valueint <= 65535)
                {
                    messageTerminate->topic = topicId->valueint;
                }
            }
            cJSON * messageLengthMax = cJSON_GetObjectItem(root, "message_length_max");
            if(cJSON_IsNumber(messageLengthMax))
            {
                if(messageLengthMax->valueint >= 3 && messageLengthMax->valueint <= 100002)
                {
                    messageTerminate->messageLengthMax = messageLengthMax->valueint;
                }
            }
            cJSON * messageId = cJSON_GetObjectItem(root, "message_id");
            if(cJSON_IsNumber(messageId))
            {
                if(messageId->valueint >= 0 && messageId->valueint <= 255)
                {
                    messageTerminate->messageId = messageId->valueint;
                }
            }
        parsed = true;
        cJSON_Delete(root);
        }
    }
    return parsed;
}

bool parseJsprUnsMessageTerminateSegment(char * jsprString, jsprMessageTerminateSegment_t * messageTerminateSegment)
{
    bool parsed = false;

    if ((jsprString != NULL) && (messageTerminateSegment != NULL))
    {
        cJSON * root = cJSON_Parse(jsprString);
        if (root != NULL)
        {
            cJSON * topicId = cJSON_GetObjectItem(root, "topic_id");
            if(cJSON_IsNumber(topicId))
            {
                if(topicId->valueint >= 64 && topicId->valueint <= 65535)
                {
                    messageTerminateSegment->topic = topicId->valueint;
                }
            }
            cJSON * segmentLength = cJSON_GetObjectItem(root, "segment_length");
            if(cJSON_IsNumber(segmentLength))
            {
                if(segmentLength->valueint >= 1 && segmentLength->valueint <= 1446)
                {
                    messageTerminateSegment->segmentLength = segmentLength->valueint;
                }
            }
            cJSON * segmentStart = cJSON_GetObjectItem(root, "segment_start");
            if(cJSON_IsNumber(segmentStart))
            {
                if(segmentStart->valueint >= 0 && segmentStart->valueint <= 100001)
                {
                    messageTerminateSegment->segmentStart = segmentStart->valueint;
                }
            }
            cJSON * messageId = cJSON_GetObjectItem(root, "message_id");
            if(cJSON_IsNumber(messageId))
            {
                if(messageId->valueint >= 0 && messageId->valueint <= 255)
                {
                    messageTerminateSegment->messageId = messageId->valueint;
                }
            }
            cJSON * data = cJSON_GetObjectItem(root, "data");
            if(cJSON_IsString(data))
            {
                memset(messageTerminateSegment->data, 0, JSPR_MAX_SEGMENT_LENGTH);
                memcpy(messageTerminateSegment->data, data->valuestring, strlen(data->valuestring));
                messageTerminateSegment->dataLength = strlen(data->valuestring);
            }
        parsed = true;
        cJSON_Delete(root);
        }
    }
    return parsed;
}

bool parseJsprGetSignal(char * jsprString, jsprConstellationState_t * signal)
{
    bool parsed = false;

    if ((jsprString != NULL) && (signal != NULL))
    {
        cJSON * root = cJSON_Parse(jsprString);
        if (root != NULL)
        {
            cJSON * constellationVisible = cJSON_GetObjectItem(root, "constellation_visible");
            if(cJSON_IsBool(constellationVisible))
            {
                signal->constellationVisible = cJSON_IsTrue(constellationVisible);
                if(signal->constellationVisible)
                {
                    cJSON * signalLevel = cJSON_GetObjectItem(root, "signal_level");
                    if(cJSON_IsNumber(signalLevel))
                    {
                        signal->signalLevel = signalLevel->valueint;
                    }
                }
            }
            cJSON * signalBars = cJSON_GetObjectItem(root, "signal_bars");
            if(cJSON_IsNumber(signalBars))
            {
                if(signalBars->valueint >= 0 && signalBars->valueint <= 5)
                {
                    signal->signalBars = signalBars->valueint;
                }
            }
        parsed = true;
        cJSON_Delete(root);
        }
    }
    return parsed;
}

bool parseJsprUnsMessageOriginateStatus(char * jsprString, jsprMessageOriginateStatus_t * messageOriginateStatus)
{
    bool parsed = false;

    if ((jsprString != NULL) && (messageOriginateStatus != NULL))
    {
        cJSON * root = cJSON_Parse(jsprString);
        if (root != NULL)
        {
            cJSON * topicId = cJSON_GetObjectItem(root, "topic_id");
            if(cJSON_IsNumber(topicId))
            {
                if(topicId->valueint >= 64 && topicId->valueint <= 65535)
                {
                    messageOriginateStatus->topic = topicId->valueint;
                }
            }
            cJSON * messageId = cJSON_GetObjectItem(root, "message_id");
            if(cJSON_IsNumber(messageId))
            {
                if(messageId->valueint >= 0 && messageId->valueint <= 255)
                {
                    messageOriginateStatus->messageId = messageId->valueint;
                }
            }
            cJSON * finalMoStatus = cJSON_GetObjectItem(root, "final_mo_status");
            if(cJSON_IsString(finalMoStatus))
            {
                if(strcmp(finalMoStatus->valuestring, "mo_ack_received") == 0)
                {
                    messageOriginateStatus->finalMoStatus = MO_ACK_RECEIVED_MOS;
                }
                else if (strcmp(finalMoStatus->valuestring, "message_discarded_on_overflow") == 0)
                {
                    messageOriginateStatus->finalMoStatus = MESSAGE_DISCARDED_ON_OVERFLOW_MOS;
                }
                else if (strcmp(finalMoStatus->valuestring, "message_expired") == 0)
                {
                    messageOriginateStatus->finalMoStatus = MESSAGE_EXPIRED_MOS;
                }
                else if (strcmp(finalMoStatus->valuestring, "message_transfer_timeout") == 0)
                {
                    messageOriginateStatus->finalMoStatus = MESSAGE_TRANSFER_TIMEOUT_MOS;
                }
                else if (strcmp(finalMoStatus->valuestring, "segment_not_supplied") == 0)
                {
                    messageOriginateStatus->finalMoStatus = SEGMENT_NOT_SUPPLIED_MOS;
                }
                else if (strcmp(finalMoStatus->valuestring, "segment_incorrect") == 0)
                {
                    messageOriginateStatus->finalMoStatus = SEGMENT_INCORRECT_MOS;
                }
                else if (strcmp(finalMoStatus->valuestring, "network_error") == 0)
                {
                    messageOriginateStatus->finalMoStatus = NETWORK_ERROR_MOS;
                }
                else if (strcmp(finalMoStatus->valuestring, "message_cancelled_pre_transit") == 0)
                {
                    messageOriginateStatus->finalMoStatus = MESSAGE_CANCELLED_PRE_TRANSIT_MOS;
                }
                else if (strcmp(finalMoStatus->valuestring, "message_cancelled_in_transit") == 0)
                {
                    messageOriginateStatus->finalMoStatus = MESSAGE_CANCELLED_IN_TRANSIT_MOS;
                }
                else if (strcmp(finalMoStatus->valuestring, "subscription_invalid") == 0)
                {
                    messageOriginateStatus->finalMoStatus = SUBSCRIPTION_INVALID_MOS;
                }
                else if (strcmp(finalMoStatus->valuestring, "protocol_error") == 0)
                {
                    messageOriginateStatus->finalMoStatus = PROTOCOL_ERROR_MOS;
                }
                else if (strcmp(finalMoStatus->valuestring, "message_dropped_local_crc_error") == 0)
                {
                    messageOriginateStatus->finalMoStatus = MESSAGE_DROPPED_LOCAL_CRC_ERROR_MOS;
                }
                else if (strcmp(finalMoStatus->valuestring, "crc_error_in_transfer") == 0)
                {
                    messageOriginateStatus->finalMoStatus = CRC_ERROR_IN_TRANSFER_MOS;
                }
                else if (strcmp(finalMoStatus->valuestring, "user_supplied_crc_error") == 0)
                {
                    messageOriginateStatus->finalMoStatus = USER_SUPPLIED_CRC_ERROR_MOS;
                }
            }
        parsed = true;
        cJSON_Delete(root);
        }
    }
    return parsed;
}

bool parseJsprUnsMessageTerminateStatus(char * jsprString, jsprMessageTerminateStatus_t * messageTerminateStatus)
{
    bool parsed = false;

    if ((jsprString != NULL) && (messageTerminateStatus != NULL))
    {
        cJSON * root = cJSON_Parse(jsprString);
        if (root != NULL)
        {
            cJSON * topicId = cJSON_GetObjectItem(root, "topic_id");
            if(cJSON_IsNumber(topicId))
            {
                if(topicId->valueint >= 64 && topicId->valueint <= 65535)
                {
                    messageTerminateStatus->topic = topicId->valueint;
                }
            }
            cJSON * messageId = cJSON_GetObjectItem(root, "message_id");
            if(cJSON_IsNumber(messageId))
            {
                if(messageId->valueint >= 0 && messageId->valueint <= 255)
                {
                    messageTerminateStatus->messageId = messageId->valueint;
                }
            }
            cJSON * finalMoStatus = cJSON_GetObjectItem(root, "final_mt_status");
            if(cJSON_IsString(finalMoStatus))
            {
                if(strcmp(finalMoStatus->valuestring, "complete") == 0)
                {
                    messageTerminateStatus->finalMtStatus = COMPLETE;
                }
                else if (strcmp(finalMoStatus->valuestring, "message_timed_out") == 0)
                {
                    messageTerminateStatus->finalMtStatus = MESSAGE_TIMED_OUT;
                }
                else if (strcmp(finalMoStatus->valuestring, "message_cancelled") == 0)
                {
                    messageTerminateStatus->finalMtStatus = MESSAGE_CANCELLED;
                }
                else if (strcmp(finalMoStatus->valuestring, "crc_error_in_transfer") == 0)
                {
                    messageTerminateStatus->finalMtStatus = CRC_ERROR_IN_TRANSFER;
                }
            }
        parsed = true;
        cJSON_Delete(root);
        }
    }
    return parsed;
}

bool parseJsprGetMessageProvisioning(char * jsprString, jsprMessageProvisioning_t * messageProvisioning)
{
        bool parsed = false;

    if ((jsprString != NULL) && (messageProvisioning != NULL))
    {
        cJSON * root = cJSON_Parse(jsprString);
        if (root != NULL)
        {
            cJSON * provisioning = cJSON_GetObjectItem(root, "provisioning");
            if(cJSON_IsArray(provisioning))
            {
                int count = cJSON_GetArraySize(provisioning);
                messageProvisioning->topicCount = count;
                for (int i = 0; i < count && i < JSPR_MAX_TOPICS; i++)
                {
                    cJSON *topic = cJSON_GetArrayItem(provisioning, i);
                    if(cJSON_IsObject(topic))
                    {
                        cJSON * topicId = cJSON_GetObjectItem(topic, "topic_id");
                        if(cJSON_IsNumber(topicId))
                        {
                            if(topicId->valueint >= 64 && topicId->valueint <= 65535)
                            {
                                messageProvisioning->provisioning[i].topicId = topicId->valueint;
                            }
                        }
                        cJSON * topicName = cJSON_GetObjectItem(topic, "topic_name");
                        if(cJSON_IsString(topicName))
                        {
                            if(strlen(topicName->valuestring) <= JSPR_TOPIC_NAME_MAX_LENGTH)
                            {
                                memset(messageProvisioning->provisioning[i].topicName, 0, JSPR_TOPIC_NAME_MAX_LENGTH);
                                memcpy(messageProvisioning->provisioning[i].topicName, topicName->valuestring, strlen(topicName->valuestring));
                            }
                        }
                        cJSON * priority = cJSON_GetObjectItem(topic, "priority");
                        if(cJSON_IsString(priority))
                        {
                            if(strcmp(priority->valuestring, "Safety-1") == 0)
                            {
                                messageProvisioning->provisioning[i].priority = SAFETY_1;
                            }
                            else if(strcmp(priority->valuestring, "Safety-2") == 0)
                            {
                                messageProvisioning->provisioning[i].priority = SAFETY_2;
                            }
                            else if(strcmp(priority->valuestring, "Safety-3") == 0)
                            {
                                messageProvisioning->provisioning[i].priority = SAFETY_3;
                            }
                            else if(strcmp(priority->valuestring, "High") == 0)
                            {
                                messageProvisioning->provisioning[i].priority = HIGH_PRIORITY;
                            }
                            else if(strcmp(priority->valuestring, "Medium") == 0)
                            {
                                messageProvisioning->provisioning[i].priority = MEDIUM_PRIORITY;
                            }
                            else if(strcmp(priority->valuestring, "Low") == 0)
                            {
                                messageProvisioning->provisioning[i].priority = LOW_PRIORITY;
                            }
                        }
                        cJSON * discardTimeSeconds = cJSON_GetObjectItem(topic, "discard_time_seconds");
                        if(cJSON_IsNumber(discardTimeSeconds))
                        {
                            if(discardTimeSeconds->valueint >= 15 && discardTimeSeconds->valueint <= 604800)
                            {
                                messageProvisioning->provisioning[i].discardTimeSeconds = discardTimeSeconds->valueint;
                            }
                        }
                        cJSON * maxQueueDepth = cJSON_GetObjectItem(topic, "max_queue_depth");
                        if(cJSON_IsNumber(maxQueueDepth))
                        {
                            if(maxQueueDepth->valueint >= 1 && maxQueueDepth->valueint <= 99)
                            {
                                messageProvisioning->provisioning[i].maxQueueDepth = maxQueueDepth->valueint;
                            }
                        }
                    }
                }
            }
        messageProvisioning->provisioningSet = true;
        parsed = true;
        cJSON_Delete(root);
        }
    }
    return parsed;
}

bool parseJsprGetHwInfo(char * jsprString, jsprHwInfo_t * hwInfo)
{
    bool parsed = false;

    if ((jsprString != NULL) && (hwInfo != NULL))
    {
        cJSON * root = cJSON_Parse(jsprString);
        if (root != NULL)
        {
            cJSON * hwVersion = cJSON_GetObjectItem(root, "hw_version");
            if(cJSON_IsString(hwVersion))
            {
                memset(hwInfo->hwVersion, 0, JSPR_HW_VERSION_MAX_LENGTH);
                memcpy(hwInfo->hwVersion, hwVersion->valuestring, JSPR_HW_VERSION_MAX_LENGTH - 1);
            }
            cJSON * serialNumber = cJSON_GetObjectItem(root, "serial_number");
            if(cJSON_IsString(serialNumber))
            {
                memset(hwInfo->serialNumber, 0, JSPR_SERIAL_NUMBER_MAX_LENGTH);
                memcpy(hwInfo->serialNumber, serialNumber->valuestring, JSPR_SERIAL_NUMBER_MAX_LENGTH - 1);
            }
            cJSON * imei = cJSON_GetObjectItem(root, "imei");
            if(cJSON_IsString(imei))
            {
                memset(hwInfo->imei, 0, JSPR_IMEI_MAX_LENGTH);
                memcpy(hwInfo->imei, imei->valuestring, JSPR_IMEI_MAX_LENGTH - 1);
            }
            cJSON * boardTemp = cJSON_GetObjectItem(root, "board_temp");
            if(cJSON_IsNumber(boardTemp))
            {
                hwInfo->boardTemp = boardTemp->valueint;
            }
        parsed = true;
        cJSON_Delete(root);
        }
    }
    return parsed;
}

bool parseJsprGetSimStatus(char * jsprString, jsprSimStatus_t * simStatus)
{
    bool parsed = false;

    if ((jsprString != NULL) && (simStatus != NULL))
    {
        cJSON * root = cJSON_Parse(jsprString);
        if (root != NULL)
        {
            cJSON * cardPresent = cJSON_GetObjectItem(root, "card_present");
            if(cJSON_IsBool(cardPresent))
            {
                simStatus->cardPresent = cJSON_IsTrue(cardPresent);
            }
            cJSON * simConnected = cJSON_GetObjectItem(root, "sim_connected");
            if(cJSON_IsBool(simConnected))
            {
                simStatus->simConnected = cJSON_IsTrue(simConnected);
            }
            cJSON * iccid = cJSON_GetObjectItem(root, "iccid");
            if(cJSON_IsString(iccid))
            {
                memset(simStatus->iccid, 0, JSPR_ICCID_MAX_LENGTH);
                memcpy(simStatus->iccid, iccid->valuestring, JSPR_ICCID_MAX_LENGTH - 1);
            }
        parsed = true;
        cJSON_Delete(root);
        }
    }
    return parsed;
}