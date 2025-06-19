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
 * This script sends a "Reflected Hello World!" message via the RB9704, then attempts
 * to read it when it gets sent back to the device.
 * 
 * A serial connection will first be attempted on the selected port, if successful a request to
 * que the message will be issued (note: this will fail if the RB9704 is not provisioned
 * for the specified topic and will block until the message is fully transferred, this might
 * take a while if the signal is poor). Once the message is sent the script will display
 * any change in signal and listen for the message to be reflected back, once the message
 * has been received an attempt to shutdown the serial connection will be made.
 * 
 * Requirements:
 * RB9704 needs to be provisioned for messaging topic 80 (reflector).
 * Have an open view of the sky where a good signal can be obtained.
 * 
*/

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

int main(int argc, char * argv[])
{
    returnCode_t rVal = SUCCESS;
    bool gotArgs = true;
    int opt = 0;
    char * mtBuffer = NULL;
    int oldSignal = 0;
    int newSignal = 0;
    _serialDevice[0] = '\0';

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
        //Begin serial connection and initialise the modem
        if(rbBegin(_serialDevice))
        {
            printf("Successfully started serial session with RB9704\r\n");
            //Queue and send MO
            const char *message = "Reflected Hello World!";
            if(rbSendMessageAny(80, message, strlen(message), 600))
            {
                printf("Sent MO: %s\r\n", message);
                //Start listening for MT
                oldSignal = rbGetSignal();
                printf("Current Signal: %d\r\n", oldSignal);
                while(_run)
                {
                    const size_t mtLength = rbReceiveMessage(&mtBuffer);
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

                    newSignal = rbGetSignal();
                    if(oldSignal != newSignal)
                    {
                        printf("Current Signal: %d\r\n", newSignal);
                        oldSignal = newSignal;
                    }

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


