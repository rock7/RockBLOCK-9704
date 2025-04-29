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
            for(size_t x = 0; x < IMT_PAYLOAD_SIZE; x++)
            {
                if(imtMoBuffer[i][x] != '\0')
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
                }
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
        for(size_t i = 0; i < MT_QUEUE_SIZE; i++)
        {
            for(size_t x = 0; x < IMT_PAYLOAD_SIZE; x++)
            {
                if(imtMtBuffer[i][x] != '\0')
                {
                    queuePosition = -1;
                }
                else
                {
                    imtMt[i].id = id; //add mt to first empty buffer
                    imtMt[i].topic = topic;
                    imtMt[i].length = length;
                    queuePosition = (int8_t)i;
                }
            }
        }
    }
    return queuePosition;
}

void removeMoFromQueue(int8_t queuePosition)
{
    memset(imtMoBuffer[queuePosition], 0, IMT_PAYLOAD_SIZE);
    imtMo[queuePosition].id = 0;
    imtMo[queuePosition].buffer = NULL;
    imtMo[queuePosition].topic = 0;
    imtMo[queuePosition].length = 0;
    imtMo[queuePosition].readyToProcess = false;
}

void removeMtFromQueue(int8_t queuePosition)
{
    memset(imtMtBuffer[queuePosition], 0, IMT_PAYLOAD_SIZE);
    imtMt[queuePosition].id = 0;
    imtMt[queuePosition].buffer = NULL;
    imtMt[queuePosition].topic = 0;
    imtMt[queuePosition].length = 0;
    imtMt[queuePosition].readyToProcess = false;
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
        imtMt[i].buffer = imtMoBuffer[i];
        memset(imtMt[i].buffer, 0, IMT_PAYLOAD_SIZE);
    }
}