#ifndef GPIO_H
#define GPIO_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#define PI_HAT_PATH "/dev/ttyS0"
#define CHIP_NAME "/dev/gpiochip0"
#define POWER_ENABLE_PIN 24U
#define IRIDIUM_ENABLE_PIN 16U
#define IRIDIUM_BOOTED_PIN 23U

#define GPIO_CHIP_MAX_LEN 20U

typedef struct
{
    const char chip[GPIO_CHIP_MAX_LEN];
    uint8_t pin;
} gpioPin_t;

typedef struct
{
    gpioPin_t powerEnable;
    gpioPin_t iridiumEnable;
    gpioPin_t booted;
} rbGpioTable_t;

extern const rbGpioTable_t gpioTable;

bool gpioToggle(const char * selectedChip, int selectedPin, int value);
bool gpioDriveHigh(const char * selectedChip, int selectedPin);
bool gpioDriveLow(const char * selectedChip, int selectedPin);
int gpioReceive(const char * selectedChip, int selectedPin);
bool gpioListenIridBooted(const char * selectedChip, int selectedPin, const int timeout);

#ifdef __cplusplus
}
#endif

#endif