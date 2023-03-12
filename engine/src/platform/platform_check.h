#ifndef PLATFORM_CHECK_H
#define PLATFORM_CHECK_H

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#ifndef _WIN64
#error 64 bit windows required!
#endif
#define CF_PLATFORM_WINDOWS
#elif defined(__linux__) || defined(__gnu_linux__)
#define CF_PLATFORM_LINUX
#elif defined(__ANDROID__)
#define CF_PLATFORM_ANDROID
#error Android is not supported yet!
#else
#error Usupported Platform!
#endif

#endif