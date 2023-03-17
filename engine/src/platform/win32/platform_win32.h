#ifndef PLATFORM_WIN32_H
#define PLATFORM_WIN32_H

#include "platform/platform_check.h"

#ifdef CF_PLATFORM_WINDOWS

#include <Windows.h>

typedef struct win32_windowData
{
    HINSTANCE instance;
    HWND handle;
} win32_windowData;

#endif

#endif