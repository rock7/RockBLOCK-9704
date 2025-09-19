#include "imt_queue.h"

static imt_queue_t imtMo;
static imt_queue_t imtMt;

static uint8_t imtMoBuffer[IMT_QUEUE_SIZE][IMT_PAYLOAD_SIZE];
static uint8_t imtMtBuffer[IMT_QUEUE_SIZE][IMT_PAYLOAD_SIZE];

static volatile bool mtLock = false;
static volatile bool moLock = true;

bool imtQueueMoAdd(uint16_t topic, const char * data, const size_t length)
{
    bool queued = false;
    uint16_t tempTail = imtMo.tail;
    if(data != NULL && length > 0)
    {
        if(imtMo.count >= imtMt.maxLength && !moLock)
        {
            imtQueueMoRemove(); //remove oldest entry if queue is full
        }

        if(imtMo.count < imtMo.maxLength)
        {
            memcpy(imtMoBuffer[tempTail], data, length);
            imtMo.messages[tempTail].topic = topic;
            imtMo.messages[tempTail].length = length;
            queued = true;

            imtMo.tail = (tempTail + 1) % imtMo.maxLength;
            imtMo.count++;
        }
    }
    return queued;
}

bool imtQueueMtAdd(const uint16_t topic, const uint16_t id, const size_t length)
{
    bool queued = false;
    uint16_t tempTail = imtMt.tail;
    if(id >= 0 && length > 0)
    {
        if(imtMt.count >= imtMt.maxLength && !mtLock)
        {
            imtQueueMtRemove(); //remove oldest entry if queue is full
        }

        if(imtMt.count < imtMt.maxLength)
        {
            imtMt.messages[tempTail].id = id;
            imtMt.messages[tempTail].topic = topic;
            imtMt.messages[tempTail].length = length;
            queued = true;

            imtMt.tail = (tempTail + 1) % imtMt.maxLength;
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

void imtQueueMoLock(bool lock)
{
    if(lock)
    {
        moLock = true;
    }
    else
    {
        moLock = false;
    }
}

imt_t * imtQueueMoGetFirst(void)
{
    imt_t * mo = NULL;

    if(imtMo.count > 0)
    {
        mo = &imtMo.messages[imtMo.head];
    }

    return mo;
}

imt_t * imtQueueMtGetFirst(void)
{
    imt_t * mt = NULL;

    if(imtMt.count > 0)
    {
        mt = &imtMt.messages[imtMt.head];
    }

    return mt;
}

imt_t * imtQueueMtGetLast(void)
{
    imt_t * mt = NULL;

    if(imtMt.count > 0)
    {
        mt = &imtMt.messages[(imtMt.tail == 0) ? (imtMt.maxLength - 1) : (imtMt.tail - 1)]; //last added message
    }

    return mt;
}

bool imtQueueMoRemove(void)
{
    bool removed = false;
    imt_t * mo = imtQueueMoGetFirst();
    if(mo != NULL)
    {
        uint16_t tempHead = imtMo.head;
        memset(mo->buffer, 0, IMT_PAYLOAD_SIZE);
        mo->id = 0;
        mo->topic = 0;
        mo->length = 0;
        mo->readyToProcess = false;
        mo->ready = false;

        imtMo.head = (tempHead + 1) % imtMo.maxLength;
        imtMo.count--;
        removed = true;
    }
    return removed;
}

bool imtQueueMtRemove(void)
{
    bool removed = false;
    imt_t * mt = imtQueueMtGetFirst();
    if(mt != NULL)
    {
        uint16_t tempHead = imtMt.head;
        memset(mt->buffer, 0, IMT_PAYLOAD_SIZE);
        mt->id = 0;
        mt->topic = 0;
        mt->length = 0;
        mt->readyToProcess = false;
        mt->ready = false;

        imtMt.head = (tempHead + 1) % imtMt.maxLength;
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