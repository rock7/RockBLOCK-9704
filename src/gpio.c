#ifdef RB_GPIO
#include "gpio.h"
#include <gpiod.h>
#include <time.h>
#include <stdio.h>

const rbGpioTable_t gpioTable = 
{
    { CHIP_NAME, POWER_ENABLE_PIN},
    { CHIP_NAME, IRIDIUM_ENABLE_PIN},
    { CHIP_NAME, IRIDIUM_BOOTED_PIN}
};

bool gpioToggle(const char * selectedChip, int selectedPin, int value)
{
    bool enabled = false;
    struct gpiod_chip * chip;
    struct gpiod_line_settings * settings;
    struct gpiod_line_info * line;
    struct gpiod_line_request * request;
    struct gpiod_line_config * config;
    int pin = selectedPin;

    chip = gpiod_chip_open(selectedChip);
    if(chip)
    {
        settings = gpiod_line_settings_new();
        if(settings)
        {
            if(gpiod_line_settings_set_direction(settings, GPIOD_LINE_DIRECTION_OUTPUT) == 0)
            {
                config = gpiod_line_config_new();
                if(config)
                {
                    if(gpiod_line_config_add_line_settings(config, &pin, 1, settings) == 0)
                    {
                        request = gpiod_chip_request_lines(chip, NULL, config);
                        if(request)
                        {
                            if(gpiod_line_request_set_value(request, pin, value) == 0)
                            {
                                enabled = true;
                            }
                        }
                        gpiod_line_request_release(request);
                    }
                }
                gpiod_line_config_free(config);
            }
        }
    }
    gpiod_chip_close(chip);
    return enabled;
}

int gpioReceive(const char * selectedChip, int selectedPin)
{
    int value = -1;
    struct gpiod_chip * chip;
    struct gpiod_line_settings * settings;
    struct gpiod_line_info * line;
    struct gpiod_line_request * request;
    struct gpiod_line_config * config;
    int pin = selectedPin;

    chip = gpiod_chip_open(selectedChip);
    if(chip)
    {
        settings = gpiod_line_settings_new();
        if(settings)
        {
            if(gpiod_line_settings_set_direction(settings, GPIOD_LINE_DIRECTION_INPUT) == 0)
            {
                config = gpiod_line_config_new();
                if(config)
                {
                    if(gpiod_line_config_add_line_settings(config, &pin, 1, settings) == 0)
                    {
                        request = gpiod_chip_request_lines(chip, NULL, config);
                        if(request)
                        {
                            value = gpiod_line_request_get_value(request, pin);
                        }
                        gpiod_line_request_release(request);
                    }
                }
                gpiod_line_config_free(config);
            }
        }
    }
    gpiod_chip_close(chip);
    return value;
}

bool gpioDriveHigh(const char * selectedChip, int selectedPin)
{
    bool enabled = false;
    if(gpioToggle(selectedChip, selectedPin, GPIOD_LINE_VALUE_ACTIVE))
    {
        enabled = true;
    }
    return enabled;
}

bool gpioDriveLow(const char * selectedChip, int selectedPin)
{
    bool disabled = false;
    if(gpioToggle(selectedChip, selectedPin, GPIOD_LINE_VALUE_INACTIVE))
    {
        disabled = true;
    }
    return disabled;
}

bool gpioListenIridBooted(const char * selectedChip, int selectedPin, const int timeout)
{
    bool enabled = true;
    time_t start = time(NULL);
    while(gpioReceive(selectedChip, selectedPin) <= 0)
    {
        //keep looping until pin iridium has booted or timeout has ran out
        if (difftime(time(NULL), start) >= timeout)
        {
            //pin hasn't gone high, break and label as false
            enabled = false;
            break;
        }
    }
    return enabled;
}
#endif