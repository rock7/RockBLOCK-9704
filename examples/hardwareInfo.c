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

//example script which populates a hwInfo structure
//with relevant modem information then prints it.

typedef enum
{
    SUCCESS = 0,
    INVALID_DEVICE,
    INVALID_ARGUMENTS,
    FAILED_TO_END_CONNECTION,
    FAILED_TO_GET_HW_VERSION,
    FAILED_TO_GET_SERIAL_NUMBER,
    FAILED_TO_GET_IMEI,
    FAILED_TO_GET_BOARD_TEMP,
    FAILED_TO_GET_CARD_PRESENT,
    FAILED_TO_GET_SIM_CONNECTED,
    FAILED_TO_GET_ICCID,
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
            //define structure and populate
            char *hwVersion = getHwVersion();
            if(hwVersion != NULL)
            {
                printf("9704 Hardware Version: %s\r\n", hwVersion);
            }
            else
            {
                printf("Failed to get hwVersion\r\n");
                rVal = FAILED_TO_GET_HW_VERSION;
                _run = false;
            }

            char *serialNumber = getSerialNumber();
            if(serialNumber != NULL)
            {
                printf("9704 Serial Number: %s\r\n", serialNumber);
            }
            else
            {
                printf("Failed to get serialNumber\r\n");
                rVal = FAILED_TO_GET_SERIAL_NUMBER;
                _run = false;
            }

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

            int8_t boardTemp = getBoardTemp();
            if(boardTemp != -100)
            {
                printf("9704 Board Temp: %d\r\n", boardTemp);
            }
            else
            {
                printf("Failed to get board temp\r\n");
                rVal = FAILED_TO_GET_BOARD_TEMP;
                _run = false;
            }

            bool cardPresent = getCardPresent();
            if(cardPresent != false)
            {
                printf("9704 card present: %s\r\n", cardPresent ? "true" : "false");
            }
            else
            {
                printf("Failed to get card present or card wasn't present\r\n");
                rVal = FAILED_TO_GET_CARD_PRESENT;
                _run = false;
            }

            bool simConnected = getSimConnected();
            if(simConnected != false)
            {
                printf("9704 SIM connected: %s\r\n", simConnected ? "true" : "false");
            }
            else
            {
                printf("Failed to get card present or card wasn't present\r\n");
                rVal = FAILED_TO_GET_SIM_CONNECTED;
                _run = false;
            }

            char *iccid = getIccid();
            if(iccid != NULL)
            {
                printf("9704 iccid: %s\r\n", iccid);
            }
            else
            {
                printf("Failed to get iccid\r\n");
                rVal = FAILED_TO_GET_ICCID;
                _run = false;
            }

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