#ifndef PLATFORM_WINDOW_H
#define PLATFORM_WINDOW_H

#include "def.h"

typedef struct PlatformWindow
{
    void* windowData;
    b8 hasClosed;
} PlatformWindow;

b8 platformCreateWindow(PlatformWindow* win, u32 x, u32 y, u32 width, u32 height, const char* title);
void platformDestroyWindow(PlatformWindow* win);
void platformWindowUpdate(PlatformWindow* win);

void getWindowSize(u32* width, u32* height);

#endif