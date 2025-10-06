#include "rockblock_9704.h"
#include <string.h>
#include <unistd.h>
#include <signal.h>

/**
 * This script uses GPIO pins rather than a USB-C cable to initialise the
 * RB9704.
 * 
 * The function to do this requires a C structure to first be defined which
 * contains the chip and pin number information of the 3 GPIO pins attached to
 * power enable, iridium enable and booted. This script will attempt to drive
 * power enable to low, then iridium enable to high which essentially turns it on.
 * It will then wait for the booted pin to be driven high, then attempt to make
 * a serial connection, if successful it will get the IMEI then start the shutdown
 * process (drive iridium enable low the power enable high).
 * 
 * Requirements:
 * Make the minimum required connections.
 * 
 *  Wiring example:
 * 
 * GND -> GND
 * VIN -> VIN
 * TX -> RX
 * RX -> TX
 * P_EN (6) -> Any free GPIO (eg 24)
 * I_EN (3) -> Any free GPIO (eg 16)
 * I_BTD (7) -> Any free GPIO (eg 23)
 * 
*/

#define PORT_PATH "/dev/ttyS0"     //path used for serial comms.
#define CHIP_NAME "/dev/gpiochip0" //chip path for selected pin.
#define POWER_ENABLE_PIN 24U       //This will be driven low on rbBeginGpio() and high on rbEndGpio().
#define IRIDIUM_ENABLE_PIN 16U     //This will be driven high on rbBeginGpio() and low on rbEndGpio().
#define IRIDIUM_BOOTED_PIN 23U     //This pin will be monitored by rbBeginGpio(), waiting until it drives high.
#define TIMEOUT 60U                //rbBeginGpio() will time out after this many seconds if IRIDIUM_BOOTED_PIN never goes high.

const rbGpioTable_t customGpioTable = 
{
    { CHIP_NAME, POWER_ENABLE_PIN},
    { CHIP_NAME, IRIDIUM_ENABLE_PIN},
    { CHIP_NAME, IRIDIUM_BOOTED_PIN}
};

static volatile bool _run = true;

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

    if(rbBeginGpio(PORT_PATH, &customGpioTable, TIMEOUT))
    {
        printf("Successfully configured GPIO for the RB9704 PiHat\r\n");
        printf("Iridium modem has booted and is ready to use\r\n");
        printf("Successfully started serial session with RB9704\r\n");
        usleep(100000); //Wait at least 100ms before queueing a message the first time you run rbBegin after boot.

        char *imei = rbGetImei();
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
        if(rbEndGpio(&customGpioTable))
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