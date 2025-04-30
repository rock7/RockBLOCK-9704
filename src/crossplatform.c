#include "crossplatform.h"

#if defined(_WIN32)
int usleep(unsigned int microseconds)
{
    Sleep(microseconds / 1000); // Sleep takes milliseconds
    return 0;
}

char * stpncpy (char * dst, const char * src, size_t len)
{
    size_t n = strlen (src);
    if (n > len)
    {
        n = len;
    }

    return strncpy (dst, src, len) + n;
}

unsigned long millis(void)
{
    return GetTickCount64();
}

void delay(uint32_t ms)
{
    Sleep(ms);
}

#elif defined(__linux__) || defined(__APPLE__)

#include <time.h>
#include <unistd.h>

unsigned long millis(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}

void delay(uint32_t ms)
{
    usleep(ms * 1000); // usleep takes microseconds
}

#endif