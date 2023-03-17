#ifndef PLATFORM_LINUX_H
#define PLATFORM_LINUX_H

#include "platform/platform_check.h"

#ifdef CF_PLATFORM_LINUX

#include <xcb/xcb.h>

typedef struct linux_windowData
{
    xcb_connection_t* connection;
    xcb_window_t window;

    xcb_intern_atom_reply_t* closeReply;
} linux_windowData;

#endif

#endif