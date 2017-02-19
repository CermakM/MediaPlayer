#ifndef PROBE_H
#define PROBE_H

#if defined _WIN64 || defined _WIN32
#include <windows.h>
#endif

#ifdef __linux__
    #define LINUX
#elif defined _WIN64 || defined _WIN32
    #define WINDOWS
#else
    #error Platform not supported
#endif


#endif // PROBE_H
