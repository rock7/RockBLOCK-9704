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

static char _serialDevice[PATH_MAX];
static volatile bool _run = true;

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

//void onMessageProvisioning(const jsprMessageProvisioning_t *msg) {
//    printf("");
//}

void onMoComplete(const unsigned int id, const int status) {
    printf("\033[1;32mMO Complete: ID = %u, Status = %d\033[0m\r\n", id, status);
}

void onMtComplete(const unsigned int id, const int status) {
    printf("\033[1;32mMT Complete: ID = %u, Status = %d\033[0m\r\n", id, status);
}

//void onConstellationState(const jsprConstellationState_t *state) {
//    printf("");
//}

int main(int argc, char * argv[])
{
    returnCode_t rVal = SUCCESS;
    bool gotArgs = true;
    int opt = 0;
    char * mtBuffer = NULL;
    _serialDevice[0] = '\0';
    int oldSignal = 0;
    int newSignal = 0;
    int messageQueued = 0;
    int messageReceived = 0;
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
        rbCallbacks_t myCallbacks =
        {
        //.messageProvisioning = onMessageProvisioning,
        .moMessageComplete = onMoComplete,
        .mtMessageComplete = onMtComplete
        //.constellationState = onConstellationState
        };

        rbRegisterCallbacks(&myCallbacks);
        //Begin serial connection and initialise the modem
        if(rbBegin(_serialDevice))
        {
            printf("Successfully started serial session with RB9704\r\n");
            //Queue and send MO
            const char *message = "hello";
            //Start listening for MT
            while(_run)
            {
                rbPoll();
                printf("\033[1;31mMessage Queued: %d\033[0m\r\n", messageQueued);
                if(messageQueued == 0)
                {
                    if(rbSendMessageAsync(message, strlen(message)))
                    {
                        printf("\033[1;32mQueued MO: %s\033[0m\r\n", message);
                        messageQueued = 1;
                    }
                }
                printf("\033[1;31mMessage Received: %d\033[0m\r\n", messageReceived);
                if(messageReceived == 0)
                {
                    const size_t mtLength = rbReceiveMessageAsync(&mtBuffer);
                    printf("\033[1;34mMessage length: %ld\033[0m\r\n", mtLength);
                    if ((mtLength > 0) && (mtBuffer != NULL))
                    {
                        printf("Received MT: ");
                        for (size_t i = 0; i < mtLength; i++)
                        {
                            // Print ascii characters
                            if ((mtBuffer[i] >= 32) && (mtBuffer[i] <= 126))
                            {
                                printf("\033[1;32m%c\033[0m", mtBuffer[i]);
                            }
                            else
                            {
                                printf(".");
                            }
                        }
                        printf("\r\n");
                        messageReceived = 1;
                    }
                }

                printf("\033[1;35mNon-Blocking!\033[0m\r\n");
                usleep(100000);
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


