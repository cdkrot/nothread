#ifndef NOTHREAD_PLATFORM_INCL
#define NOTHREAD_PLATFORM_INCL

#if !defined(_WIN32) && (defined(__unix__) || defined(__unix) || (defined(__APPLE__) && defined(__MACH__)))
#include <unistd.h>
#define NOTHREAD_PLATFORM NIX
#define NOTHREAD_PLATFORM_MMAP_SUPPORTED
#define NOTHREAD_PLATFORM_STACKGROWTH DOWN

#else

#error "Your system is not supported or we failed to detect it"

#endif

#endif
