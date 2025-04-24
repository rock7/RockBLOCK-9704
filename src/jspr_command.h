#ifndef JSPR_COMMAND_H
#define JSPR_COMMAND_H

#ifdef __cplusplus
extern "C" {
#endif

#include "jspr.h"
#include "crossplatform.h"
#include <stdbool.h>

#define COMMAND_MAX_LEN 2048U

bool jsprGetApiVersion(void);
bool jsprPutApiVersion(const jsprDottedVersion_t * apiVersion);
bool jsprGetSimInterface(void);
bool jsprPutSimInterface(const char * iface);
bool jsprGetOperationalState(void);
bool jsprPutOperationalState(const char * state);
bool jsprPutMessageOriginate(const uint16_t topic, const size_t length);
bool jsprPutMessageOriginateSegment(jsprMessageOriginate_t * messageOriginate, const size_t segmentLength, uint32_t segmentStart, const char * data);
bool jsprGetSignal(void);
bool jsprGetMessageProvisioning(void);
bool jsprGetHwInfo(void);
bool jsprGetFirmware(const jsprBootSource_t slot);
bool jsprPutFirmware(const jsprBootSource_t slot);
bool jsprGetSimStatus(void);

bool putSimInterface(availableSimInterfaces_t iface);
bool putOperationalState(availableOperationalStates_t state);

#ifdef __cplusplus
}
#endif

#endif