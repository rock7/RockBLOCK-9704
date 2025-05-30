#include "imt_queue.h"

imt_t imtMo[MO_QUEUE_SIZE] = { {0, NULL, 0, 0, false} };
imt_t imtMt[MT_QUEUE_SIZE] = { {0, NULL, 0, 0, false} };

static uint8_t imtMoBuffer[MO_QUEUE_SIZE][IMT_PAYLOAD_SIZE];
static uint8_t imtMtBuffer[MT_QUEUE_SIZE][IMT_PAYLOAD_SIZE];

int8_t addMoToQueue(uint16_t topic, const char * data, const size_t length)
{
    int8_t queuePosition = -1;
    if(data != NULL && length > 0)
    {
        for(size_t i = 0; i < MO_QUEUE_SIZE; i++)
        {
            if(imtMo[i].length != 0)
            {
                queuePosition = -1;
            }
            else
            {
                memcpy(imtMoBuffer[i], data, length); //add mo to first empty buffer
                imtMo[i].buffer = imtMoBuffer[i];
                imtMo[i].topic = topic;
                imtMo[i].length = length;
                queuePosition = (int8_t)i;
                break;
            }
        }
    }
    return queuePosition;
}

int8_t addMtToQueue(uint16_t topic, uint16_t id, size_t length)
{
    int8_t queuePosition = -1;
    if(id >= 0 && length > 0)
    {
        if(imtMt[0].length != 0 && imtMt[MT_QUEUE_SIZE - 1].length == 0) //keep shifting messages up unless queue is full
        {
            shiftMtQueueDown();
        }
        for(size_t i = 0; i < MT_QUEUE_SIZE; i++)
        {
            if(imtMt[i].length != 0)
            {
                queuePosition = -1;
            }
            else
            {
                imtMt[i].id = id; //add mt to first empty buffer, should always be 0 or -1
                imtMt[i].topic = topic;
                imtMt[i].length = length;
                queuePosition = (int8_t)i;
                break;
            }
        }
    }
    return queuePosition;
}

bool shiftMoQueueUp(void)
{
    bool success = false;
    if(imtMo[0].length == 0) //check that head of que is empty before shifting
    {
        for (size_t i = 1; i < MO_QUEUE_SIZE; i++)
        {
            memcpy(imtMoBuffer[i - 1], imtMoBuffer[i], IMT_PAYLOAD_SIZE);
            imtMo[i - 1] = imtMo[i];
            imtMo[i - 1].buffer = imtMoBuffer[i - 1];
        }

        removeMoFromQueue(MO_QUEUE_SIZE - 1);
        success = true;
    }
    return success;
}

bool shiftMtQueueUp(void)
{
    bool success = false;
    if(imtMt[0].buffer == NULL) //check that head of que is empty before shifting
    {
        for (size_t i = 1; i < MT_QUEUE_SIZE; i++)
        {
            memcpy(imtMtBuffer[i - 1], imtMtBuffer[i], IMT_PAYLOAD_SIZE);
            imtMt[i - 1] = imtMt[i];
            imtMt[i - 1].buffer = imtMtBuffer[i - 1];
        }

        removeMtFromQueue(MT_QUEUE_SIZE - 1);
        success = true;
    }
    return success;
}

void shiftMtQueueDown(void)
{
    if(imtMt[MT_QUEUE_SIZE - 1].length != 0) //check that the tail of que is empty before shifting
    {
        removeMtFromQueue(MT_QUEUE_SIZE - 1); //clear oldest MT if queue full
    }

    for (size_t i = 1; i < MT_QUEUE_SIZE; i++)
    {
        memcpy(imtMtBuffer[i], imtMtBuffer[i - 1], IMT_PAYLOAD_SIZE);
        imtMt[i] = imtMt[i - 1];
        imtMt[i].buffer = imtMtBuffer[i];
    }
    removeMtFromQueue(0);
}

void removeMoFromQueue(int8_t queuePosition)
{
    memset(imtMoBuffer[queuePosition], 0, IMT_PAYLOAD_SIZE);
    imtMo[queuePosition].id = 0;
    imtMo[queuePosition].topic = 0;
    imtMo[queuePosition].length = 0;
    imtMo[queuePosition].readyToProcess = false;
    imtMo[queuePosition].ready = false;
}

void removeMtFromQueue(int8_t queuePosition)
{
    memset(imtMtBuffer[queuePosition], 0, IMT_PAYLOAD_SIZE);
    imtMt[queuePosition].id = 0;
    imtMt[queuePosition].topic = 0;
    imtMt[queuePosition].length = 0;
    imtMt[queuePosition].readyToProcess = false;
    imtMt[queuePosition].ready = false;
}

void imtQueueInit (void) //zero out all queue buffers
{
    for (size_t i = 0; i < MO_QUEUE_SIZE; i++)
    {
        imtMo[i].buffer = imtMoBuffer[i];
        memset(imtMo[i].buffer, 0, IMT_PAYLOAD_SIZE);
    }

    for (size_t i = 0; i < MT_QUEUE_SIZE; i++)
    {
        imtMt[i].buffer = imtMtBuffer[i];
        memset(imtMt[i].buffer, 0, IMT_PAYLOAD_SIZE);
    }
}