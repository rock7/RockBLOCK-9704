#include "gtest/gtest.h"
#include "jspr_command.h"
#include "serial_test.h"
#include "serial.h"

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

#define SERIAL_CONTEXT_SETUP_FUNC testSerialInterface_setContext

class jspr_command_test : public testing::Test
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

TEST_F ( jspr_command_test, commandGetApi )
{
    testSerialInterface_resetBuffers();
    const char apiCommand[] =  "GET apiVersion {}\r";
    const uint8_t * outputBuffer = testSerialInterface_getOutputBuffer();
    EXPECT_TRUE(jsprGetApiVersion());
    EXPECT_STREQ((const char *)outputBuffer, apiCommand);
}

TEST_F ( jspr_command_test, commandPutApi )
{
    const char apiCommand[] =  "PUT apiVersion {\"active_version\": {\"major\": 1, \"minor\": 5, \"patch\": 5}}\r";
    jsprDottedVersion_t apiVersion;
    apiVersion.major = 1;
    apiVersion.minor = 5;
    apiVersion.patch = 5;

    const uint8_t * outputBuffer = testSerialInterface_getOutputBuffer();

    EXPECT_TRUE(jsprPutApiVersion(&apiVersion));
    EXPECT_STREQ((const char *)outputBuffer, apiCommand);
}

TEST_F ( jspr_command_test, commandGetSimInterface )
{
    const char simCommand[] =  "GET simConfig {}\r";

    const uint8_t * outputBuffer = testSerialInterface_getOutputBuffer();

    EXPECT_TRUE(jsprGetSimInterface());
    EXPECT_STREQ((const char *)outputBuffer, simCommand);
}

TEST_F ( jspr_command_test, commandPutSimInterface )
{
    const char simCommand[] =  "PUT simConfig {\"interface\": \"internal\"}\r";

    const uint8_t * outputBuffer = testSerialInterface_getOutputBuffer();

    EXPECT_TRUE(putSimInterface(INTERNAL));
    EXPECT_STREQ((const char *)outputBuffer, simCommand);
}

TEST_F ( jspr_command_test, commandGetOperationalState )
{
    const char operationalStateCommand[] =  "GET operationalState {}\r";

    const uint8_t * outputBuffer = testSerialInterface_getOutputBuffer();

    EXPECT_TRUE(jsprGetOperationalState());
    EXPECT_STREQ((const char *)outputBuffer, operationalStateCommand);
}

TEST_F ( jspr_command_test, commandPutOperationalState )
{
    const char operationalStateCommand[] =  "PUT operationalState {\"state\": \"active\"}\r";

    const uint8_t * outputBuffer = testSerialInterface_getOutputBuffer();

    EXPECT_TRUE(putOperationalState(ACTIVE));
    EXPECT_STREQ((const char *)outputBuffer, operationalStateCommand);
}

TEST_F ( jspr_command_test, commandPutMessageOriginate)
{
    const char messageOriginateCommand[] =  "PUT messageOriginate {\"topic_id\":244, \"message_length\":12, \"request_reference\":1}\r";
    uint16_t topic = 244;
    size_t length = 12;

    const uint8_t * outputBuffer = testSerialInterface_getOutputBuffer();

    EXPECT_TRUE(jsprPutMessageOriginate(topic, length));
    EXPECT_STREQ((const char *)outputBuffer, messageOriginateCommand);
}