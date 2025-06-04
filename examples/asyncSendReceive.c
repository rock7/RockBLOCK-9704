#include "rockblock_9704.h"
#include <string.h>
#include <signal.h>
#include <getopt.h>
#include <limits.h>
#include "crossplatform.h"

#if defined(_WIN32)
#include <io.h>
#define access _access
#else
#include <unistd.h>
#endif

/**
 * This script showcases the asynchronous and queuing capabilites of this library by setting
 * the necessary callbacks, queuing and sending 5 messages and listening for any incoming messages
 * without blocking.
 * 
 * At the start of the script we setup our 4 user defined callbacks to get message provisioning,
 * check if our queued messages have sent, check if we received any messages and finally check the
 * signal strength. A serial connection will then be attempted on the selected port. The script will call
 * rbPoll() everytime it loops, it is important that this is done quite frequently (10ms in this example)
 * as that function is responsible for listening to all the replies from the modem. Five messages will be
 * queued at the start and should start sending if a good signal has been obtained. The callback for message
 * provisioning should then return all the provisioned topics, followed by the onMoComplete callback for 
 * every message sent. The onConstellationState callback will be called anytime either the signal level or
 * signal bars have changed although in this example we will only print the change in signal bars. Finally
 * the script will constantly wait for incoming messages, by waiting for the onMtComplete callback to be called,
 * then store and acknowledge them to clear space for others.
 * 
 * Requirements:
 * RB9704 needs to be provisioned for messaging topic 244 (RAW).
 * Have an open view of the sky where a good signal can be obtained.
 * Adjust MO_QUEUE_SIZE in imt_queue.h to 5U.
 * 
 * (OPTIONAL) If you want to use and test MT queuing do the following:
 * Adjust MT_QUEUE_SIZE in imt_queue.h to desired queue size.
 * Remove or adjust rbAcknowledgeReceiveHeadAsync() so that messages aren't acknowledged right away.
 * Use rbReceiveLockAsync() if you want to lock the queue so that no messages are removed when queue is full.
 * Use rbReceiveUnlockAsync() if you want to unlock the queue.
 * 
*/

static char _serialDevice[PATH_MAX];
static volatile bool _run = true;

int messagesSent = 0;
int currentSignal = 0;
bool receivedNewMessage = false;

typedef enum
{
    SUCCESS = 0,
    INVALID_DEVICE,
    INVALID_ARGUMENTS,
    FAILED_TO_END_CONNECTION,
    FAILED_QUEUE_MESSAGE,
    FAILED_INIT_SERIAL,
} returnCode_t;

static struct option _longOptions[] =
{
    {"device", required_argument, 0, 'd'},
    {"help", no_argument, 0, 'h'},
    {0, 0, 0, 0}
};

static void printHelp(const char * progName)
{
    printf("Usage: %s -d <device> [-h]\n", progName);
    printf("  -d, --device   Serial device to use (mandatory)\n");
    printf("  -h, --help     Display this help message\n");
}

static void signal_handler(int sig)
{
    (void)sig;
    _run = false;
}

void onMessageProvisioning(const jsprMessageProvisioning_t *messageProvisioning)
{
    if(messageProvisioning->provisioningSet == true)
    {
        printf("\033[1;32mDevice is provisioned for %d topics\033[0m\r\n", messageProvisioning->topicCount);
        printf("\033[1;32mProvisioned topics:\033[0m\r\n");
        for(int i = 0; i < messageProvisioning->topicCount; i++)
        {
            printf("\033[1;32mTopic name: %s Topic number: %d\033[0m\r\n", 
            messageProvisioning->provisioning[i].topicName, messageProvisioning->provisioning[i].topicId);
        }
    }
}

void onMoComplete(const unsigned int id, const int status)
{
    printf("\033[1;32mMO Complete: ID = %u, Status = %d\033[0m\r\n", id, status);
    if(status == 1)
    {
        messagesSent += 1;
    }
}

void onMtComplete(const unsigned int id, const int status)
{
    printf("\033[1;32mMT Complete: ID = %u, Status = %d\033[0m\r\n", id, status);
    if(status == 1)
    {
        receivedNewMessage = true;
    }
}

void onConstellationState(const jsprConstellationState_t *state)
{
    if(state->signalBars != currentSignal)
    {
        printf("\033[1;34mCurrent Signal: %d\033[0m\r\n", state->signalBars);
        currentSignal = state->signalBars;
    }
}

int main(int argc, char * argv[])
{
    returnCode_t rVal = SUCCESS;
    bool gotArgs = true;
    int opt = 0;
    _serialDevice[0] = '\0';
    char * mtBuffer = NULL;
    char mtStore[100000];
    int messagesQueued = 0;
    int messagesReceived = 0;
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    while ((opt = getopt_long(argc, argv, "d:h", _longOptions, NULL)) != -1)
    {
        switch (opt)
        {
            case 'd':
                strncpy(_serialDevice, optarg, PATH_MAX -1);
            break;

            case 'h':
                printHelp(argv[0]);
                gotArgs = false;
            break;

            case '?':
            // fall through
            default:
                printHelp(argv[0]);
                rVal = INVALID_ARGUMENTS;
            break;
        }
    }
    if ((rVal == SUCCESS) && (gotArgs == true))
    {
        //Assign callbacks
        rbCallbacks_t myCallbacks =
        {
        .messageProvisioning = onMessageProvisioning,
        .moMessageComplete = onMoComplete,
        .mtMessageComplete = onMtComplete,
        .constellationState = onConstellationState
        };
        //Register Callbacks
        rbRegisterCallbacks(&myCallbacks);
        //Begin serial connection and initialise the modem
        if(rbBegin(_serialDevice))
        {
            printf("Successfully started serial session with RB9704\r\n");
            //Queue and send 5 messages
            const char *message = "Message 0";
            const char *message1 = "Message 1";
            const char *message2 = "Message 2";
            const char *message3 = "Message 3";
            const char *message4 = "Message 4";
            //Start listening for MT
            while(_run)
            {
                rbPoll();
                if(messagesQueued == 0)
                {
                    if(rbSendMessageAsync(RAW_TOPIC, message, strlen(message)))
                    {
                        printf("\033[1;32mQueued MO: %s\033[0m\r\n", message);
                        messagesQueued += 1;
                    }
                    
                    if(rbSendMessageAsync(RAW_TOPIC, message1, strlen(message1)))
                    {
                        printf("\033[1;32mQueued MO: %s\033[0m\r\n", message1);
                        messagesQueued += 1;
                    }
                    
                    if(rbSendMessageAsync(RAW_TOPIC, message2, strlen(message2)))
                    {
                        printf("\033[1;32mQueued MO: %s\033[0m\r\n", message2);
                        messagesQueued += 1;
                    }
                    if(rbSendMessageAsync(RAW_TOPIC, message3, strlen(message3)))
                    {
                        printf("\033[1;32mQueued MO: %s\033[0m\r\n", message3);
                        messagesQueued += 1;
                    }
                    if(rbSendMessageAsync(RAW_TOPIC, message4, strlen(message4)))
                    {
                        printf("\033[1;32mQueued MO: %s\033[0m\r\n", message4);
                        messagesQueued += 1;
                    }
                    printf("\033[1;33mMessage Queued: %d\033[0m\r\n", messagesQueued);
                    printf("\033[1;33mMessage Sent: %d\033[0m\r\n", messagesSent);
                }
                //Wait for MT callback then get the message from the head of that queue
                if(receivedNewMessage)
                {
                    receivedNewMessage = false;
                    const size_t mtLength = rbReceiveMessageAsync(&mtBuffer);
                    printf("\033[1;33mMessage length: %ld\033[0m\r\n", mtLength);
                    if ((mtLength > 0) && (mtBuffer != NULL))
                    {
                        //Store the message in another location as we will be clearing it from the queue
                        memcpy(mtStore, mtBuffer, mtLength);
                        printf("Received MT: ");
                        for (size_t i = 0; i < mtLength; i++)
                        {
                            // Print ascii characters
                            if ((mtStore[i] >= 32) && (mtStore[i] <= 126))
                            {
                                printf("\033[1;32m%c\033[0m", mtStore[i]);
                            }
                            else
                            {
                                printf(".");
                            }
                        }
                        printf("\r\n");
                        memset(mtStore, 0, sizeof(mtStore));
                        messagesReceived += 1;
                        printf("\033[1;33mMessages Received: %d\033[0m\r\n", messagesReceived);
                        //Clear the message from the queue to make space
                        if(rbAcknowledgeReceiveHeadAsync())
                        {
                            printf("\033[1;34mMessage acknowledged\033[0m\r\n");
                        }
                    }
                }
                //put cool app stuff here!
                
                //rbPoll() needs to be called very frequently
                usleep(10000);
            }

            //End serial connection
            if(rbEnd())
            {
                printf("Ended connection successfully\r\n");
                _run = false;
            }
            else
            {
                printf("Failed to end connection\r\n");
                rVal = FAILED_TO_END_CONNECTION;
                _run = false;
            }
        }
        else
        {
            printf("Failed to begin the serial connection\r\n");
            rVal = FAILED_INIT_SERIAL;
            _run = false;
        }
    }

    return rVal;
}


