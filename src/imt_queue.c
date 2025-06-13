#include "imt_queue.h"

static imt_queue_t imtMo;
static imt_queue_t imtMt;

static uint8_t imtMoBuffer[IMT_QUEUE_SIZE][IMT_PAYLOAD_SIZE];
static uint8_t imtMtBuffer[IMT_QUEUE_SIZE][IMT_PAYLOAD_SIZE];

static bool mtLock = false;

int8_t imtQueueMoPush(uint16_t topic, const char * data, const size_t length)
{
    bool queued = false;
    uint16_t tempTail = imtMo.tail;
    if(data != NULL && length > 0)
    {
        if(imtMo.count < imtMo.maxLength)
        {
            memcpy(imtMoBuffer[tempTail], data, length);
            //imtMo.messages[tempTail].buffer = imtMoBuffer[imtMo.head];
            imtMo.messages[tempTail].topic = topic;
            imtMo.messages[tempTail].length = length;
            queued = true;

            imtMo.tail = (tempTail + 1) % IMT_QUEUE_SIZE;
            imtMo.count++;
        }
    }
    return queued;
}

int8_t imtQueueMtPush(uint16_t topic, uint16_t id, size_t length)
{
    bool queued = false;
    uint16_t tempTail = imtMt.tail;
    if(id >= 0 && length > 0)
    {
        if(imtMt.count >= imtMt.maxLength && !mtLock)
        {
            imtQueueRemoveMt(); //remove oldest entry if queue is full
        }

        if(imtMt.count < imtMt.maxLength)
        {
            imtMt.messages[tempTail].id = id;
            imtMt.messages[tempTail].topic = topic;
            imtMt.messages[tempTail].length = length;
            queued = true;

            imtMt.tail = (tempTail + 1) % IMT_QUEUE_SIZE;
            imtMt.count++;
        }
    }
    return queued;
}

void imtQueueMtLock(bool lock)
{
    if(lock)
    {
        mtLock = true;
    }
    else
    {
        mtLock = false;
    }
}

imt_t * imtQueueMoPeek(void)
{
    imt_t * mo = NULL;

    if(imtMo.count > 0)
    {
        mo = &imtMo.messages[imtMo.head];
    }

    return mo;
}

imt_t * imtQueueMtPeek(void)
{
    imt_t * mt = NULL;

    if(imtMt.count > 0)
    {
        mt = &imtMt.messages[imtMt.head];
    }

    return mt;
}

bool imtQueueRemoveMo(void)
{
    bool removed = false;
    imt_t * mo = imtQueueMoPeek();
    if(mo != NULL)
    {
        uint16_t tempHead = imtMo.head;
        memset(mo->buffer, 0, IMT_PAYLOAD_SIZE);
        mo->id = 0;
        mo->topic = 0;
        mo->length = 0;
        mo->readyToProcess = false;
        mo->ready = false;

        imtMo.head = (tempHead + 1) % IMT_QUEUE_SIZE;
        imtMo.count--;
        removed = true;
    }
    return removed;
}

bool imtQueueRemoveMt(void)
{
    bool removed = false;
    imt_t * mt = imtQueueMtPeek();
    if(mt != NULL)
    {
        uint16_t tempHead = imtMt.head;
        memset(mt->buffer, 0, IMT_PAYLOAD_SIZE);
        mt->id = 0;
        mt->topic = 0;
        mt->length = 0;
        mt->readyToProcess = false;
        mt->ready = false;

        imtMt.head = (tempHead + 1) % IMT_QUEUE_SIZE;
        imtMt.count--;
        removed = true;
    }
    return removed;
}

void imtQueueInit(void)
{
    for (uint16_t i = 0; i < IMT_QUEUE_SIZE; i++)
    {
        imtMo.messages[i].buffer = imtMoBuffer[i];
        memset(imtMo.messages[i].buffer, 0, IMT_PAYLOAD_SIZE);
        imtMo.messages[i].id = 0;
        imtMo.messages[i].length = 0;
        imtMo.messages[i].ready = false;
        imtMo.messages[i].readyToProcess = false;
        imtMo.messages[i].topic = 0;


        imtMt.messages[i].buffer = imtMtBuffer[i];
        memset(imtMt.messages[i].buffer, 0, IMT_PAYLOAD_SIZE);
        imtMt.messages[i].id = 0;
        imtMt.messages[i].length = 0;
        imtMt.messages[i].ready = false;
        imtMt.messages[i].readyToProcess = false;
        imtMt.messages[i].topic = 0;
    }

    imtMo.head = 0;
    imtMo.tail = 0;
    imtMo.count = 0;
    imtMo.maxLength = IMT_QUEUE_SIZE;

    imtMt.head = 0;
    imtMt.tail = 0;
    imtMt.count = 0;
    imtMt.maxLength = IMT_QUEUE_SIZE;

}