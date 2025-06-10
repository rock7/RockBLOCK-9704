#ifndef IMT_QUEUE_H
#define IMT_QUEUE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#ifndef IMT_QUEUE_SIZE
    #define IMT_QUEUE_SIZE 1U
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

typedef struct
{
    imt_t messages[IMT_QUEUE_SIZE];
    uint16_t count;
    uint16_t head;
    uint16_t tail;
    uint16_t maxLength;
} imt_queue_t;

int8_t imtQueueMoPush(uint16_t topic, const char * data, const size_t length);
int8_t imtQueueMtPush(uint16_t topic, uint16_t id, size_t length);
bool imtQueueRemoveMo(void);
bool imtQueueRemoveMt(void);
imt_t * imtQueueMoPeek(void);
imt_t * imtQueueMtPeek(void);
void imtQueueMtLock(bool lock);
void imtQueueInit (void);

#ifdef __cplusplus
}
#endif

#endif