#include <string.h>
#include "serial.h"
#include "serial_test.h"

#define BUFFER_SIZE 8192

static uint8_t g_inputBuffer[BUFFER_SIZE];
static uint8_t g_outputBuffer[BUFFER_SIZE];
static uint16_t g_inputIndex = 0;
static uint16_t g_outputIndex = 0;
static uint16_t g_bytesAvailable = 0;
unsigned int g_serialInitCallCount = 0;

serialContext context =
{
    NULL, // serialInit
    NULL, // serialDeInit
    NULL, // serialRead
    NULL, // serialWrite
};

bool testSerialInterface_setContext(void)
{
    bool set = false;

    context.serialInit = testSerialInterface_serialInitialise;;
    context.serialDeInit = testSerialInterface_serialDeInitialise;
    context.serialRead = testSerialInterface_serialReadFunc;
    context.serialWrite = testSerialInterface_serialWriteFunc;

    if(context.serialInit()) //Open and close the port to test
    {
        if(context.serialDeInit())
        {
            set = true;
        }
    }
    return set;
}

bool testSerialInterface_serialInitialise (void)
{
    g_serialInitCallCount++;
    return true;
}

bool testSerialInterface_serialDeInitialise (void)
{
    return true;
}

uint16_t testSerialInterface_serialBytesAvailableFunc (void)
{
    return g_bytesAvailable;
}

int testSerialInterface_serialReadFunc (char * bytes, const uint16_t length)
{
    int i = 0;
    for (i = 0; i < (int)length; i++)
    {
        if ((g_inputIndex < BUFFER_SIZE) && (g_bytesAvailable > 0))
        {
            bytes[i] = g_inputBuffer[g_inputIndex++];
            g_bytesAvailable--;
        }
        else
        {
            break;
        }
    }
    return i;
}

int testSerialInterface_serialWriteFunc (const char * data, const uint16_t length)
{
    int bytesWritten = 0;
    while (*data != '\0' && g_outputIndex < BUFFER_SIZE && g_outputIndex < length)
    {
        g_outputBuffer[g_outputIndex++] = *data++;
        bytesWritten++;
    }
    return bytesWritten;
}

void testSerialInterface_setInputBuffer(const uint8_t *data, int size)
{
    for (int i = 0; i < size && i < BUFFER_SIZE; i++)
    {
        g_inputBuffer[i] = data[i];
        g_bytesAvailable++;
    }
}

const uint8_t *testSerialInterface_getOutputBuffer(void)
{
    return g_outputBuffer;
}

void testSerialInterface_resetOutputBuffer(void)
{
    memset(g_outputBuffer, '\0', sizeof(g_outputBuffer));
    g_outputIndex = 0;
}

void testSerialInterface_resetInputBuffer(void)
{
    memset(g_inputBuffer, '\0', sizeof(g_inputBuffer));
    g_inputIndex = 0;
}

void testSerialInterface_resetBuffers (void)
{
    testSerialInterface_resetOutputBuffer();
    testSerialInterface_resetInputBuffer();
}