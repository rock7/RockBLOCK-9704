#include "rockblock_9704.h"
#include <string.h>
#include <signal.h>
#include <getopt.h>
#include <limits.h>
#include <stdlib.h>
#include "crossplatform.h"

#if defined(_WIN32)
#include <io.h>
#define access _access
#else
#include <unistd.h>
#endif

static char _serialDevice[PATH_MAX];
static char _message[PATH_MAX];
static int _topic;
static volatile bool _run = true;

//example script which sends a user defined message
//on a user defined topic, through a user defined port.

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
    {"topic", required_argument, 0, 't'},
    {"message", required_argument, 0, 'm'},
    {"help", no_argument, 0, 'h'},
    {0, 0, 0, 0}
};

static void printHelp(const char * progName)
{
    printf("Usage: %s -d <device> [-h]\n", progName);
    printf("  -d, --device   Serial device to use (mandatory)\n");
    printf("  -t, --topic   Topic on which the message will be sent (mandatory)\n");
    printf("  -m, --message   Message/payload to be sent (mandatory)\n");
    printf("  -h, --help     Display this help message\n");
}

static void signal_handler(int sig)
{
    (void)sig;
    _run = false;
}

int main(int argc, char * argv[])
{
    returnCode_t rVal = SUCCESS;
    bool gotArgs = true;
    int opt = 0;
    char * mtBuffer = NULL;
    int oldSignal = 0;
    int newSignal = 0;
    _serialDevice[0] = '\0';
    _message[0] = '\0';
    _topic = 244; 

    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    while ((opt = getopt_long(argc, argv, "d:t:m:h", _longOptions, NULL)) != -1)
    {
        switch (opt)
        {
            case 'd':
                strncpy(_serialDevice, optarg, PATH_MAX -1);
            break;

            case 't':
                _topic = atoi(optarg);
            break;

            case 'm':
                strncpy(_message, optarg, PATH_MAX -1);
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
        //Begin serial connection and initialise the modem
        if(rbBegin(_serialDevice))
        {
            printf("Successfully started serial session with RB9704\r\n");
            //Queue and send MO
            if(sendMessageAny(_topic, _message, strlen(_message)))
            {
                printf("Sent MO: %s\r\n", _message);
                //Start listening for MT
                oldSignal = getSignal();
                printf("Current Signal: %d\r\n", oldSignal);
                while(_run)
                {
                    const size_t mtLength = receiveMessage(&mtBuffer);
                    if ((mtLength > 0) && (mtBuffer != NULL))
                    {
                        printf("Received MT: ");
                        for (size_t i = 0; i < mtLength; i++)
                        {
                            // Print ascii characters
                            if ((mtBuffer[i] >= 32) && (mtBuffer[i] <= 126))
                            {
                                printf("%c", mtBuffer[i]);
                            }
                            else
                            {
                                printf(".");
                            }
                        }
                        printf("\r\n");
                        break; //Break out of loop if MT is found
                    }

                    newSignal = getSignal();
                    if(oldSignal != newSignal)
                    {
                        printf("Current Signal: %d\r\n", newSignal);
                        oldSignal = newSignal;
                    }

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
                printf("Failed to queue message\r\n");
                rVal = FAILED_QUEUE_MESSAGE;
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


