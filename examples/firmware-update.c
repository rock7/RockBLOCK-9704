#include "rockblock_9704.h"
#include <string.h>
#include <stdbool.h>
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
 * This script will take a valid specified 9704 firmware file and proceed
 * to update the modem on the specified port.
 * 
 * Requirements:
 * Valid 9704 FW file.
 * 
*/

#define PROGRESS_BAR_WIDTH 60U

static char _serialDevice[PATH_MAX];
static char _firmwareFile[PATH_MAX];

typedef enum
{
    SUCCESS = 0,
    INVALID_DEVICE,
    INVALID_FIRMWARE_FILE,
    INVALID_ARGUMENTS,
    FAILED_INIT_SERIAL,
    FIRMWARE_UPDATE_FAILED,
} returnCode_t;

static struct option _longOptions[] =
{
    {"device", required_argument, 0, 'd'},
    {"file",   required_argument, 0, 'f'},
    {"help",   no_argument, 0, 'h'},
    {0, 0, 0, 0}
};

static void printHelp(const char * progName)
{
    printf("Usage: %s -d <device> -f <firmware file> [-h]\n", progName);
    printf("  -d, --device   Serial device to use (mandatory)\n");
    printf("  -f, --file     Iridium 9700 series firmware file (mandatory)\n");
    printf("  -h, --help     Display this help message\n");
}

static void signal_handler(int sig)
{
    (void)sig;
#if defined (_WIN32)
    ExitProcess(SUCCESS);
#else
    kill(getpid(), SIGHUP);  // Send SIGHUP to self
#endif
}

static void progressCallback(const unsigned long sofar, const unsigned long total)
{
    if ((sofar > 0 ) && (total > 0))
    {
        const float progress = (float)sofar / (float)total;
        if (progress <= 1.0)
        {
            fprintf(stdout, "[");
            const int pos = PROGRESS_BAR_WIDTH * progress;
            for (int i = 0; i < PROGRESS_BAR_WIDTH; i++)
            {
                if (i < pos)
                {
                    fprintf(stdout, "=");
                }
                else if (i == pos)
                {
                    fprintf(stdout, ">");
                }
                else
                {
                    fprintf(stdout, " ");
                }
            }
            fprintf(stdout, "] %d %% (%ld / %ld)", (int)(progress * 100.0), sofar, total);

            if (isatty(fileno(stdout)))
            {
                fprintf(stdout, "\r");  // Move to the beginning of the line
            }
            else
            {
                fprintf(stdout, "\n");  // New line for non-terminal output
            }
            fflush(stdout);
        }

        if (progress >= 1.0)
        {
            fprintf(stdout, "\n");
        }
    }
}

int main(int argc, char * argv[])
{
    returnCode_t rVal = SUCCESS;
    bool gotArgs = true;
    int opt = 0;
    _serialDevice[0] = '\0';
    _firmwareFile[0] = '\0';

    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    while ((opt = getopt_long(argc, argv, "d:f:h", _longOptions, NULL)) != -1)
    {
        switch (opt)
        {
            case 'd':
                strncpy(_serialDevice, optarg, PATH_MAX -1);
            break;

            case 'f':
                strncpy(_firmwareFile, optarg, PATH_MAX -1);
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
            printf("Current Firmware Version: %s\n", getFirmwareVersion());

            if (updateFirmware (_firmwareFile, progressCallback) == true)
            {
                printf("Successfully update the firmware, wait for the RockBLOCK 9704 to reboot\n");
            }
            else
            {
                printf("Failed to updated firmware\n");
                rVal = FIRMWARE_UPDATE_FAILED;
            }

        }
        else
        {
            printf("Failed to begin the serial connection\n");
            rVal = FAILED_INIT_SERIAL;
        }
    }

    return rVal;
}


