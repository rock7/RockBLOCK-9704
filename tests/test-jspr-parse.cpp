#include "gtest/gtest.h"
#include "jspr.h"
#include "serial_test.h"
#include "serial.h"

#define SERIAL_CONTEXT_SETUP_FUNC testSerialInterface_setContext

class jspr_parse_test : public testing::Test
{
    protected:

        void SetUp() override
        {
            // Code here will be called immediately after the constructor (right
            // before each test).
            SERIAL_CONTEXT_SETUP_FUNC();
            testSerialInterface_resetBuffers();
         }
};

TEST_F ( jspr_parse_test, parseBootInfo )
{
    const char bootInfoJson[] =  R"({"image_type":"production","version":{"major":1,"minor":5,"patch":5,"build_info":"SX-240927-6C70C408-P1-P3142-1025485-11216251"},"boot_source":"primary"})";
    std::string bootInfoJspr = "299 bootInfo ";
    bootInfoJspr.append(bootInfoJson);
    bootInfoJspr.append("\r");

    testSerialInterface_setInputBuffer((const uint8_t *)bootInfoJspr.data(), bootInfoJspr.length());

    jsprResponse_t response;

    EXPECT_TRUE(receiveJspr(&response, "bootInfo"));
    EXPECT_EQ(response.code,299);
    EXPECT_STREQ(response.target,"bootInfo");
    EXPECT_STREQ(response.json, bootInfoJson);

    jsprBootInfo_t bootInfo;
    memset(&bootInfo, 0, sizeof(jsprBootInfo_t));

    ASSERT_TRUE(parseJsprBootInfo(response.json, &bootInfo));
    EXPECT_STREQ(bootInfo.imageType, "production");
    EXPECT_EQ(bootInfo.bootSource, JSPR_BOOT_SOURCE_PRIMARY);
    EXPECT_EQ(bootInfo.versionInfo.version.major, 1);
    EXPECT_EQ(bootInfo.versionInfo.version.minor, 5);
    EXPECT_EQ(bootInfo.versionInfo.version.patch, 5);
    EXPECT_STREQ(bootInfo.versionInfo.buildInfo, "SX-240927-6C70C408-P1-P3142-1025485-11216251");
}

TEST_F( jspr_parse_test, parseGetApiVersion )
{
    const char apiVersionJson[] = R"({"supported_versions":[{"major":1,"minor":0,"patch":1},{"major":1,"minor":1,"patch":0},{"major":1,"minor":2,"patch":0}],"active_version":{"major":1,"minor":2,"patch":0}})";
    std::string getApiVersionJspr = "200 apiVersion ";
    getApiVersionJspr.append(apiVersionJson);
    getApiVersionJspr.append("\r");

    testSerialInterface_setInputBuffer((const uint8_t *)getApiVersionJspr.data(), getApiVersionJspr.length());

    jsprResponse_t response;

    EXPECT_TRUE(receiveJspr(&response, "apiVersion"));
    EXPECT_EQ(response.code,200);
    EXPECT_STREQ(response.target,"apiVersion");
    EXPECT_STREQ(response.json, apiVersionJson);

    jsprApiVersion_t apiVersion;
    memset(&apiVersion, 0, sizeof(jsprApiVersion_t));

    ASSERT_TRUE(parseJsprGetApiVersion(response.json, &apiVersion));
    EXPECT_EQ(apiVersion.supportedVersionCount, 2);
    EXPECT_EQ(apiVersion.supportedVersions[0].major, 1);
    EXPECT_EQ(apiVersion.supportedVersions[0].minor, 2);
    EXPECT_EQ(apiVersion.supportedVersions[0].patch, 0);
    EXPECT_EQ(apiVersion.supportedVersions[1].major, 1);
    EXPECT_EQ(apiVersion.supportedVersions[1].minor, 1);
    EXPECT_EQ(apiVersion.supportedVersions[1].patch, 0);
    EXPECT_EQ(apiVersion.activeVersion.major, 1);
    EXPECT_EQ(apiVersion.activeVersion.minor, 2);
    EXPECT_EQ(apiVersion.activeVersion.patch, 0);
    EXPECT_TRUE(apiVersion.activeVersionSet); 
}

TEST_F( jspr_parse_test, parseFirmware )
{
    const char firmwareJson[] = R"({"slot":"primary","validity":true,"version":{"major":1,"minor":5,"patch":5,"build_info":"SX-240927-6C70C408-P1-P3142-1025485-11216251"},"hash":"a82923dbc0745c19ccd1c81d85e9a8ced9875f7f648db9bacd8d19bab61090cb"})";
    std::string firmwareJspr = "200 firmware ";
    firmwareJspr.append(firmwareJson);
    firmwareJspr.append("\r");

    testSerialInterface_setInputBuffer((const uint8_t *)firmwareJspr.data(), firmwareJspr.length());

    jsprResponse_t response;

    EXPECT_TRUE(receiveJspr(&response, "firmware"));
    EXPECT_EQ(response.code,200);
    EXPECT_STREQ(response.target,"firmware");
    EXPECT_STREQ(response.json, firmwareJson);

    jsprFirmwareInfo_t firmwareInfo;
    memset(&firmwareInfo, 0, sizeof(jsprFirmwareInfo_t));

    ASSERT_TRUE(parseJsprFirmwareInfo(response.json, &firmwareInfo));
    EXPECT_EQ(firmwareInfo.slot, JSPR_BOOT_SOURCE_PRIMARY);
    EXPECT_TRUE(firmwareInfo.validity);
    EXPECT_EQ(firmwareInfo.versionInfo.version.major, 1);
    EXPECT_EQ(firmwareInfo.versionInfo.version.minor, 5);
    EXPECT_EQ(firmwareInfo.versionInfo.version.patch, 5);
    EXPECT_STREQ(firmwareInfo.versionInfo.buildInfo, "SX-240927-6C70C408-P1-P3142-1025485-11216251");
    EXPECT_STREQ(firmwareInfo.hash, "a82923dbc0745c19ccd1c81d85e9a8ced9875f7f648db9bacd8d19bab61090cb");
}

TEST_F( jspr_parse_test, parseGetSimInterface )
{
    const char simInterfaceJson[] = R"({"interface":"internal"})";
    std::string simInterfaceJspr = "200 simConfig ";
    simInterfaceJspr.append(simInterfaceJson);
    simInterfaceJspr.append("\r");

    testSerialInterface_setInputBuffer((const uint8_t *)simInterfaceJspr.data(), simInterfaceJspr.length());

    jsprResponse_t response;

    EXPECT_TRUE(receiveJspr(&response, "simConfig"));
    EXPECT_EQ(response.code,200);
    EXPECT_STREQ(response.target,"simConfig");
    EXPECT_STREQ(response.json, simInterfaceJson);

    jsprSimInterface_t simInterfaceInfo;
    memset(&simInterfaceInfo, 0, sizeof(jsprSimInterface_t));

    ASSERT_TRUE(parseJsprGetSimInterface(response.json, &simInterfaceInfo));
    EXPECT_EQ(simInterfaceInfo.iface, INTERNAL);
    EXPECT_TRUE(simInterfaceInfo.ifaceSet);
}

TEST_F( jspr_parse_test, parseGetOperationalState )
{
    const char operationalStateJson[] = R"({"state":"active","reason":0})";
    std::string operationalStateJspr = "200 operationalState ";
    operationalStateJspr.append(operationalStateJson);
    operationalStateJspr.append("\r");

    testSerialInterface_setInputBuffer((const uint8_t *)operationalStateJspr.data(), operationalStateJspr.length());

    jsprResponse_t response;

    EXPECT_TRUE(receiveJspr(&response, "operationalState"));
    EXPECT_EQ(response.code,200);
    EXPECT_STREQ(response.target,"operationalState");
    EXPECT_STREQ(response.json, operationalStateJson);

    jsprOperationalState_t operationalStateInfo;
    memset(&operationalStateInfo, 0, sizeof(jsprOperationalState_t));

    ASSERT_TRUE(parseJsprGetOperationalState(response.json, &operationalStateInfo));
    EXPECT_EQ(operationalStateInfo.operationalState, ACTIVE);
    EXPECT_EQ(operationalStateInfo.reason, NORMAL);
    EXPECT_TRUE(operationalStateInfo.operationalStateSet);
}

TEST_F( jspr_parse_test, parsePutMessageOriginate )
{
    const char messageOriginateJson[] = R"({"topic_id":244,"request_reference":1,"message_id":4,"message_response":"message_accepted"})";
    std::string messageOriginateJspr = "200 messageOriginate ";
    messageOriginateJspr.append(messageOriginateJson);
    messageOriginateJspr.append("\r");

    testSerialInterface_setInputBuffer((const uint8_t *)messageOriginateJspr.data(), messageOriginateJspr.length());

    jsprResponse_t response;

    EXPECT_TRUE(receiveJspr(&response, "messageOriginate"));
    EXPECT_EQ(response.code,200);
    EXPECT_STREQ(response.target,"messageOriginate");
    EXPECT_STREQ(response.json, messageOriginateJson);

    jsprMessageOriginate_t messageOriginateInfo;
    memset(&messageOriginateInfo, 0, sizeof(jsprMessageOriginate_t));

    ASSERT_TRUE(parseJsprPutMessageOriginate(response.json, &messageOriginateInfo));
    EXPECT_EQ(messageOriginateInfo.messageResponse, MESSAGE_ACCEPTED);
    EXPECT_EQ(messageOriginateInfo.messageId, 4);
    EXPECT_EQ(messageOriginateInfo.requestReference, 1);
    EXPECT_EQ(messageOriginateInfo.topic, 244);
    EXPECT_TRUE(messageOriginateInfo.messageIdSet);
}

TEST_F( jspr_parse_test, parseUnsMessageOriginateSegment )
{
    const char messageOriginateSegmentJson[] = R"({"topic_id":244,"message_id":4,"segment_length":7,"segment_start":0})";
    std::string messageOriginateSegmentJspr = "299 messageOriginateSegment ";
    messageOriginateSegmentJspr.append(messageOriginateSegmentJson);
    messageOriginateSegmentJspr.append("\r");

    testSerialInterface_setInputBuffer((const uint8_t *)messageOriginateSegmentJspr.data(), messageOriginateSegmentJspr.length());

    jsprResponse_t response;

    EXPECT_TRUE(receiveJspr(&response, "messageOriginateSegment"));
    EXPECT_EQ(response.code,299);
    EXPECT_STREQ(response.target,"messageOriginateSegment");
    EXPECT_STREQ(response.json, messageOriginateSegmentJson);

    jsprMessageOriginateSegment_t messageOriginateSegmentInfo;
    memset(&messageOriginateSegmentInfo, 0, sizeof(jsprMessageOriginateSegment_t));

    ASSERT_TRUE(parseJsprUnsMessageOriginateSegment(response.json, &messageOriginateSegmentInfo));
    EXPECT_EQ(messageOriginateSegmentInfo.segmentStart, 0);
    EXPECT_EQ(messageOriginateSegmentInfo.segmentLength, 7);
    EXPECT_EQ(messageOriginateSegmentInfo.messageId, 4);
    EXPECT_EQ(messageOriginateSegmentInfo.topic, 244);
}

TEST_F( jspr_parse_test, parseUnsMessageTerminate )
{
    const char messageTerminateJson[] = R"({"topic_id":244,"message_id":1,"message_length_max":7})";
    std::string messageTerminateJspr = "299 messageTerminate ";
    messageTerminateJspr.append(messageTerminateJson);
    messageTerminateJspr.append("\r");

    testSerialInterface_setInputBuffer((const uint8_t *)messageTerminateJspr.data(), messageTerminateJspr.length());

    jsprResponse_t response;

    EXPECT_TRUE(receiveJspr(&response, "messageTerminate"));
    EXPECT_EQ(response.code,299);
    EXPECT_STREQ(response.target,"messageTerminate");
    EXPECT_STREQ(response.json, messageTerminateJson);

    jsprMessageTerminate_t messageTerminateInfo;
    memset(&messageTerminateInfo, 0, sizeof(jsprMessageTerminate_t));

    ASSERT_TRUE(parseJsprUnsMessageTerminate(response.json, &messageTerminateInfo));
    EXPECT_EQ(messageTerminateInfo.messageId, 1);
    EXPECT_EQ(messageTerminateInfo.messageLengthMax, 7);
    EXPECT_EQ(messageTerminateInfo.topic, 244);
}

TEST_F( jspr_parse_test, parseUnsMessageTerminateSegment )
{
    const char messageTerminateSegmentJson[] = R"({"topic_id":244,"message_id":1,"segment_length":7,"segment_start":0,"data":"aGVsbG/DYg=="})";
    std::string messageTerminateSegmentJspr = "299 messageTerminateSegment ";
    messageTerminateSegmentJspr.append(messageTerminateSegmentJson);
    messageTerminateSegmentJspr.append("\r");

    testSerialInterface_setInputBuffer((const uint8_t *)messageTerminateSegmentJspr.data(), messageTerminateSegmentJspr.length());

    jsprResponse_t response;

    EXPECT_TRUE(receiveJspr(&response, "messageTerminateSegment"));
    EXPECT_EQ(response.code,299);
    EXPECT_STREQ(response.target,"messageTerminateSegment");
    EXPECT_STREQ(response.json, messageTerminateSegmentJson);

    jsprMessageTerminateSegment_t messageTerminateSegmentInfo;
    memset(&messageTerminateSegmentInfo, 0, sizeof(jsprMessageTerminateSegment_t));

    ASSERT_TRUE(parseJsprUnsMessageTerminateSegment(response.json, &messageTerminateSegmentInfo));
    EXPECT_EQ(messageTerminateSegmentInfo.messageId, 1);
    EXPECT_EQ(messageTerminateSegmentInfo.segmentLength, 7);
    EXPECT_EQ(messageTerminateSegmentInfo.segmentStart, 0);
    EXPECT_EQ(messageTerminateSegmentInfo.topic, 244);
    EXPECT_STREQ(messageTerminateSegmentInfo.data,"aGVsbG/DYg==");
}

TEST_F( jspr_parse_test, parseGetSignal )
{
    const char getSignalJson[] = R"({"constellation_visible":true, "signal_bars":5,"signal_level":-108})";
    std::string getSignalJspr = "200 constellationState ";
    getSignalJspr.append(getSignalJson);
    getSignalJspr.append("\r");

    testSerialInterface_setInputBuffer((const uint8_t *)getSignalJspr.data(), getSignalJspr.length());

    jsprResponse_t response;

    EXPECT_TRUE(receiveJspr(&response, "constellationState"));
    EXPECT_EQ(response.code,200);
    EXPECT_STREQ(response.target,"constellationState");
    EXPECT_STREQ(response.json, getSignalJson);

    jsprConstellationState_t getSignalInfo;
    memset(&getSignalInfo, 0, sizeof(jsprConstellationState_t));

    ASSERT_TRUE(parseJsprGetSignal(response.json, &getSignalInfo));
    EXPECT_EQ(getSignalInfo.signalBars, 5);
    EXPECT_EQ(getSignalInfo.signalLevel, -108);
    EXPECT_TRUE(getSignalInfo.constellationVisible);
}

TEST_F( jspr_parse_test, parseUnsMessageOriginateStatus )
{
    const char messageOriginateStatusJson[] = R"({"topic_id":244, "message_id":4,"final_mo_status":"mo_ack_received"})";
    std::string messageOriginateStatusJspr = "299 messageOriginateStatus ";
    messageOriginateStatusJspr.append(messageOriginateStatusJson);
    messageOriginateStatusJspr.append("\r");

    testSerialInterface_setInputBuffer((const uint8_t *)messageOriginateStatusJspr.data(), messageOriginateStatusJspr.length());

    jsprResponse_t response;

    EXPECT_TRUE(receiveJspr(&response, "messageOriginateStatus"));
    EXPECT_EQ(response.code,299);
    EXPECT_STREQ(response.target,"messageOriginateStatus");
    EXPECT_STREQ(response.json, messageOriginateStatusJson);

    jsprMessageOriginateStatus_t messageOriginateStatusInfo;
    memset(&messageOriginateStatusInfo, 0, sizeof(jsprMessageOriginateStatus_t));

    ASSERT_TRUE(parseJsprUnsMessageOriginateStatus(response.json, &messageOriginateStatusInfo));
    EXPECT_EQ(messageOriginateStatusInfo.finalMoStatus, MO_ACK_RECEIVED_MOS);
    EXPECT_EQ(messageOriginateStatusInfo.messageId, 4);
    EXPECT_EQ(messageOriginateStatusInfo.topic, 244);
}

TEST_F( jspr_parse_test, parseUnsMessageTerminateStatus )
{
    const char messageTerminateStatusJson[] = R"({"topic_id":244, "message_id":4,"final_mt_status":"complete"})";
    std::string messageTerminateStatusJspr = "299 messageTerminateStatus ";
    messageTerminateStatusJspr.append(messageTerminateStatusJson);
    messageTerminateStatusJspr.append("\r");

    testSerialInterface_setInputBuffer((const uint8_t *)messageTerminateStatusJspr.data(), messageTerminateStatusJspr.length());

    jsprResponse_t response;

    EXPECT_TRUE(receiveJspr(&response, "messageTerminateStatus"));
    EXPECT_EQ(response.code,299);
    EXPECT_STREQ(response.target,"messageTerminateStatus");
    EXPECT_STREQ(response.json, messageTerminateStatusJson);

    jsprMessageTerminateStatus_t messageTerminateStatusInfo;
    memset(&messageTerminateStatusInfo, 0, sizeof(jsprMessageTerminateStatus_t));

    ASSERT_TRUE(parseJsprUnsMessageTerminateStatus(response.json, &messageTerminateStatusInfo));
    EXPECT_EQ(messageTerminateStatusInfo.finalMtStatus, COMPLETE);
    EXPECT_EQ(messageTerminateStatusInfo.messageId, 4);
    EXPECT_EQ(messageTerminateStatusInfo.topic, 244);
}

TEST_F( jspr_parse_test, parseGetMessageProvisioning )
{
    const char messageProvisioningJson[] = R"({"provisioning":[{"topic_id":244,"topic_name":"RAW","priority":"High","discard_time_seconds":604800,"max_queue_depth":99},{"topic_id":247,"topic_name":"PEBBLE","priority":"Medium","discard_time_seconds":432000,"max_queue_depth":99}]})";
    std::string messageProvisioningJspr = "200 messageProvisioning ";
    messageProvisioningJspr.append(messageProvisioningJson);
    messageProvisioningJspr.append("\r");

    testSerialInterface_setInputBuffer((const uint8_t *)messageProvisioningJspr.data(), messageProvisioningJspr.length());

    jsprResponse_t response;

    EXPECT_TRUE(receiveJspr(&response, "messageProvisioning"));
    EXPECT_EQ(response.code,200);
    EXPECT_STREQ(response.target,"messageProvisioning");
    EXPECT_STREQ(response.json, messageProvisioningJson);

    jsprMessageProvisioning_t messageProvisioningInfo;
    memset(&messageProvisioningInfo, 0, sizeof(jsprMessageProvisioning_t));

    ASSERT_TRUE(parseJsprGetMessageProvisioning(response.json, &messageProvisioningInfo));
    EXPECT_STREQ(messageProvisioningInfo.provisioning[0].topicName, "RAW");
    EXPECT_EQ(messageProvisioningInfo.provisioning[0].topicId, 244);
    EXPECT_EQ(messageProvisioningInfo.provisioning[0].priority, HIGH_PRIORITY);
    EXPECT_EQ(messageProvisioningInfo.provisioning[0].discardTimeSeconds, 604800);
    EXPECT_EQ(messageProvisioningInfo.provisioning[0].maxQueueDepth, 99);
    EXPECT_STREQ(messageProvisioningInfo.provisioning[1].topicName, "PEBBLE");
    EXPECT_EQ(messageProvisioningInfo.provisioning[1].topicId, 247);
    EXPECT_EQ(messageProvisioningInfo.provisioning[1].priority, MEDIUM_PRIORITY);
    EXPECT_EQ(messageProvisioningInfo.provisioning[1].discardTimeSeconds, 432000);
    EXPECT_EQ(messageProvisioningInfo.provisioning[1].maxQueueDepth, 99);
}

TEST_F( jspr_parse_test, parseGetHwInfo )
{
    const char hwInfoJson[] = R"({"hw_version":"0x0601","serial_number":"1a01cw","imei":"300258060209940","board_temp":24})";
    std::string hwInfoJspr = "200 hwInfo ";
    hwInfoJspr.append(hwInfoJson);
    hwInfoJspr.append("\r");

    testSerialInterface_setInputBuffer((const uint8_t *)hwInfoJspr.data(), hwInfoJspr.length());

    jsprResponse_t response;

    EXPECT_TRUE(receiveJspr(&response, "hwInfo"));
    EXPECT_EQ(response.code,200);
    EXPECT_STREQ(response.target,"hwInfo");
    EXPECT_STREQ(response.json, hwInfoJson);

    jsprHwInfo_t hwInfoInfo;
    memset(&hwInfoInfo, 0, sizeof(jsprHwInfo_t));

    ASSERT_TRUE(parseJsprGetHwInfo(response.json, &hwInfoInfo));
    EXPECT_STREQ(hwInfoInfo.hwVersion, "0x0601");
    EXPECT_STREQ(hwInfoInfo.serialNumber, "1a01cw");
    EXPECT_STREQ(hwInfoInfo.imei, "300258060209940");
    EXPECT_EQ(hwInfoInfo.boardTemp, 24);
}

TEST_F( jspr_parse_test, parseGetSimStatus )
{
    const char simStatusJson[] = R"({"card_present":true,"sim_connected":true,"iccid":"8988169771000758053"})";
    std::string simStatusJspr = "200 simStatus ";
    simStatusJspr.append(simStatusJson);
    simStatusJspr.append("\r");

    testSerialInterface_setInputBuffer((const uint8_t *)simStatusJspr.data(), simStatusJspr.length());

    jsprResponse_t response;

    EXPECT_TRUE(receiveJspr(&response, "simStatus"));
    EXPECT_EQ(response.code,200);
    EXPECT_STREQ(response.target,"simStatus");
    EXPECT_STREQ(response.json, simStatusJson);

    jsprSimStatus_t simStatusInfo;
    memset(&simStatusInfo, 0, sizeof(jsprSimStatus_t));

    ASSERT_TRUE(parseJsprGetSimStatus(response.json, &simStatusInfo));
    EXPECT_TRUE(simStatusInfo.cardPresent);
    EXPECT_TRUE(simStatusInfo.simConnected);
    EXPECT_STREQ(simStatusInfo.iccid, "8988169771000758053");
}