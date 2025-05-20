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
    memset(imtMo, 0, sizeof(imt_t));

    queuePosition = addMoToQueue(topic, data, length);
    ASSERT_EQ(queuePosition, 0);
    EXPECT_STREQ((const char *)imtMo[0].buffer, data);
    EXPECT_EQ(imtMo[0].topic, 244);
    EXPECT_EQ(imtMo[0].length, 12);
}