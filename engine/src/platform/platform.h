#ifndef PLATFORM_H
#define PLATFORM_H

#include "def.h"

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#define PLATFORM_WINDOWS
#elif defined(__linux__) || defined(__gnu_linux__)
#define PLATFORM_LINUX
#elif defined(__ANDROID__)
#define PLATFORM_ANDROID
#error Android not supported yet!
#else
#error Unsupported platform!
#endif

typedef struct PlatformWindow
{
    void* windowData;
} PlatformWindow;

b8 platformCreateWindow(PlatformWindow* win, u32 x, u32 y, u32 w, u32 h, const char* title);
void platformDestroyWindow(PlatformWindow* win);
void platformWindowUpdate(PlatformWindow* win);

void platformWriteConsole(u32 color, const char* msg);
f64 platformGetTime();

// VUlkan platform functions
#include <vulkan/vulkan.h>

const char* getPlatformSurfaceExtenion();
VkSurfaceKHR createSurface(PlatformWindow* win, VkInstance instance, VkAllocationCallbacks* allocator);

#endif