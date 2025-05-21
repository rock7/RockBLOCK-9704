#include "gtest/gtest.h"
#include "imt_queue.h"

class imt_queue_test : public testing::Test
{
    protected:

        void SetUp() override
        {
            // Code here will be called immediately after the constructor (right
            // before each test).
            //SERIAL_CONTEXT_SETUP_FUNC();
            //testSerialInterface_resetBuffers();
         }
};

TEST_F ( imt_queue_test, addMoQueue )
{
    const char * data = "Hello world!";
    const size_t length = 12;
    int8_t queuePosition = -1;
    uint16_t topic = 244;

    extern imt_t imtMo[MO_QUEUE_SIZE];

    for (size_t i = 0; i < MO_QUEUE_SIZE; i++)
    {
        memset(imtMo, 0, sizeof(imt_t));

        queuePosition = addMoToQueue(topic, data, length);
        ASSERT_EQ(queuePosition, i);
        EXPECT_STREQ((const char *)imtMo[i].buffer, data);
        EXPECT_EQ(imtMo[i].topic, 244);
        EXPECT_EQ(imtMo[i].length, 12);
    }
}

TEST_F ( imt_queue_test, addMtQueue )
{
    uint16_t id = 1;
    const size_t length = 12;
    int8_t queuePosition = -1;
    uint16_t topic = 244;

    extern imt_t imtMt[MT_QUEUE_SIZE];

    for (size_t i = 0; i < MT_QUEUE_SIZE; i++)
    {
        memset(imtMt, 0, sizeof(imt_t));

        queuePosition = addMtToQueue(topic, id, length);
        ASSERT_EQ(queuePosition, i);
        EXPECT_EQ(imtMt[i].topic, 244);
        EXPECT_EQ(imtMt[i].id, 1);
        EXPECT_EQ(imtMt[i].length, 12);
    }
}

TEST_F ( imt_queue_test, removeMoQueue )
{
    uint8_t testBuffer[IMT_PAYLOAD_SIZE] = {0};
    uint8_t * oldBuffer;

    extern imt_t imtMo[MO_QUEUE_SIZE];

    for (size_t i = 0; i < MO_QUEUE_SIZE; i++)
    {
        oldBuffer = imtMo[i].buffer;
        imtMo[i].readyToProcess = true;

        removeMoFromQueue(i);
        EXPECT_EQ(memcmp(oldBuffer, testBuffer, IMT_PAYLOAD_SIZE), 0);
        EXPECT_EQ(imtMo[i].id, 0);
        EXPECT_EQ(imtMo[i].buffer, nullptr);
        EXPECT_EQ(imtMo[i].topic, 0);
        EXPECT_EQ(imtMo[i].length, 0);
        EXPECT_FALSE(imtMo[i].readyToProcess);
    }
}

TEST_F ( imt_queue_test, removeMtQueue )
{
    uint8_t testBuffer[IMT_PAYLOAD_SIZE] = {0};
    const char * data = "Hello world!";
    uint8_t * oldBuffer;

    extern imt_t imtMt[MT_QUEUE_SIZE];
    extern uint8_t imtMtBuffer[MT_QUEUE_SIZE][IMT_PAYLOAD_SIZE];

    for (size_t i = 0; i < MT_QUEUE_SIZE; i++)
    {
        memcpy(imtMtBuffer[i], data, 12);
        imtMt[i].buffer = imtMtBuffer[i];
        oldBuffer = imtMt[i].buffer;
        imtMt[i].readyToProcess = true;

        removeMtFromQueue(i);
        EXPECT_EQ(memcmp(oldBuffer, testBuffer, IMT_PAYLOAD_SIZE), 0);
        EXPECT_EQ(imtMt[i].id, 0);
        EXPECT_EQ(imtMt[i].buffer, nullptr);
        EXPECT_EQ(imtMt[i].topic, 0);
        EXPECT_EQ(imtMt[i].length, 0);
        EXPECT_FALSE(imtMt[i].readyToProcess);
    }
}

TEST_F ( imt_queue_test, initQueue )
{
    uint8_t testBuffer[IMT_PAYLOAD_SIZE] = {0};
    extern imt_t imtMo[MO_QUEUE_SIZE];
    extern imt_t imtMt[MT_QUEUE_SIZE];
    extern uint8_t imtMtBuffer[MT_QUEUE_SIZE][IMT_PAYLOAD_SIZE];
    extern uint8_t imtMoBuffer[MO_QUEUE_SIZE][IMT_PAYLOAD_SIZE];

    for (size_t i = 0; i < MO_QUEUE_SIZE; i++)
    {
        imtMo[i].buffer = imtMoBuffer[i];
        memset(imtMo[i].buffer, 1, IMT_PAYLOAD_SIZE);
    }

    for (size_t i = 0; i < MT_QUEUE_SIZE; i++)
    {
        imtMt[i].buffer = imtMtBuffer[i];
        memset(imtMt[i].buffer, 1, IMT_PAYLOAD_SIZE);
    }

    imtQueueInit();

    for (size_t i = 0; i < MO_QUEUE_SIZE; i++)
    {
        EXPECT_EQ(memcmp(imtMo[i].buffer, testBuffer, IMT_PAYLOAD_SIZE), 0);
    }

    for (size_t i = 0; i < MT_QUEUE_SIZE; i++)
    {
        EXPECT_EQ(memcmp(imtMt[i].buffer, testBuffer, IMT_PAYLOAD_SIZE), 0);
    }
}