#ifndef PLATFORM_H
#define PLATFORM_H

#include "def.h"

#include "platform_window.h"

#define MAX_WINDOW_COUNT 0xff

typedef struct PlatformState
{
    PlatformWindow windows[MAX_WINDOW_COUNT];
} PlatformState;

#endif