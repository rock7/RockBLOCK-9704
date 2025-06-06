#ifndef IMT_QUEUE_H
#define IMT_QUEUE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#ifndef MO_QUEUE_SIZE
    #define MO_QUEUE_SIZE 1U
#endif

#ifndef MT_QUEUE_SIZE
    #define MT_QUEUE_SIZE 1U
#endif

#define IMT_CRC_SIZE 2U
#ifdef ARDUINO
#define IMT_PAYLOAD_SIZE 5000U + IMT_CRC_SIZE
#else
#define IMT_PAYLOAD_SIZE 100000U + IMT_CRC_SIZE
#endif

typedef struct
{
    uint16_t id; // Message ID from messageOriginate response or mobileTerminate unsolicited
    uint8_t * buffer;
    size_t length;
    uint16_t topic;
    bool readyToProcess;
    bool ready;
} imt_t;

int8_t addMoToQueue(uint16_t topic, const char * data, const size_t length);
int8_t addMtToQueue(uint16_t topic, uint16_t id, size_t length);
void removeMoFromQueue(int8_t queuePosition);
void removeMtFromQueue(int8_t queuePosition);
void imtQueueInit (void);
bool shiftMoQueueUp(void);
bool shiftMtQueueUp(void);
void shiftMtQueueDown(void);

#ifdef __cplusplus
}
#endif

#endif