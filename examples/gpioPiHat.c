#include "rockblock_9704.h"
#include <string.h>
#include <unistd.h>
#include <signal.h>

static volatile bool _run = true;

//example script which configures the gpio
//to work with the RB9704 PiHat, begins connection
//and checks the modems IMEI to confirm successful
//communication.

typedef enum
{
    SUCCESS = 0,
    INVALID_ARGUMENTS,
    FAILED_TO_END_CONNECTION,
    FAILED_TO_GET_IMEI,
    FAILED_INIT_GPIO,
} returnCode_t;

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

    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    if(rbBeginHat(60))
    {
        printf("Successfully configured GPIO for the RB9704 PiHat\r\n");
        printf("Iridium modem has booted and is ready to use\r\n");
        printf("Successfully started serial session with RB9704\r\n");

        char *imei = getImei();
        if(imei != NULL)
        {
            printf("9704 Imei: %s\r\n", imei);
        }
        else
        {
            printf("Failed to get imei\r\n");
            rVal = FAILED_TO_GET_IMEI;
            _run = false;
        }
        if(rbEndHat())
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
        printf("Failed to configure GPIO for the RB9704 PiHat\r\n");
        rVal = FAILED_INIT_GPIO;
        _run = false;
    }

    return rVal;
}


