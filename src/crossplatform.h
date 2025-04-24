#ifndef CROSSPLATFORM_H
#define CROSSPLATFORM_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_WIN32)
    #include <windows.h>
    #include <string.h>

    #ifndef PATH_MAX
        #define PATH_MAX MAX_PATH
    #endif

    int usleep(unsigned int microseconds);

    char * stpncpy (char * dst, const char * src, size_t len);

#endif


#ifdef __cplusplus
}
#endif

#endif // CROSSPLATFORM_H
