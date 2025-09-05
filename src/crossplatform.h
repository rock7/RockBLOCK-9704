#ifndef CROSSPLATFORM_H
#define CROSSPLATFORM_H

#ifdef ARDUINO
#include <Arduino.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_WIN32)
    #include <windows.h>
    #include <string.h>
    #include <stdint.h>

    #ifndef PATH_MAX
        #define PATH_MAX MAX_PATH
    #endif

    int usleep(unsigned int microseconds);

    char * stpncpy (char * dst, const char * src, size_t len);

    unsigned long millis(void);
    void delay(uint32_t ms);
#elif defined(__linux__) || defined(__APPLE__)
    #include <stdint.h>

    unsigned long millis(void);
    void delay(uint32_t ms);
#endif

#ifdef __cplusplus
}
#endif

#endif // CROSSPLATFORM_H
