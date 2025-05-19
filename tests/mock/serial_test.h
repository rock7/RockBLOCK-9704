#ifndef TEST_SERIAL_INTERFACE_H
#define TEST_SERIAL_INTERFACE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

extern unsigned int g_serialInitCallCount;

bool testSerialInterface_setContext(void);
bool testSerialInterface_serialInitialise (void);
bool testSerialInterface_serialDeInitialise (void);
uint16_t testSerialInterface_serialBytesAvailableFunc (void);
int testSerialInterface_serialReadFunc (char * bytes, const uint16_t length);
int testSerialInterface_serialWriteFunc (const char * data, const uint16_t length);
void testSerialInterface_setInputBuffer(const uint8_t *data, int size);
const uint8_t *testSerialInterface_getOutputBuffer(void);
void testSerialInterface_resetOutputBuffer(void);
void testSerialInterface_resetInputBuffer(void);
void testSerialInterface_resetBuffers(void);

#ifdef __cplusplus
}
#endif

#endif