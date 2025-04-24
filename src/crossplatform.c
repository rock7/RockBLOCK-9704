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

#endif