#include "platform.h"

#ifdef PLATFORM_LINUX

#include <xcb/xcb.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>

#include "core/cf_memory.h"
#include "core/logger.h"

typedef struct WindowData
{
    xcb_connection_t* connection;
    xcb_window_t window;
} WindowData;

b8 platformCreateWindow(PlatformWindow* win, u32 x, u32 y, u32 w, u32 h, const char* title)
{
    win->windowData = cfAllocate(sizeof(WindowData));
    WindowData* data = (WindowData*)win->windowData;

    data->connection = xcb_connect(PNULL, PNULL);

    const xcb_setup_t* setup = xcb_get_setup(data->connection);
    xcb_screen_iterator_t iter = xcb_setup_roots_iterator(setup);
    xcb_screen_t* screen = iter.data;

    u32 values[] = {XCB_EVENT_MASK_STRUCTURE_NOTIFY};

    data->window = xcb_generate_id(data->connection);
    xcb_create_window(data->connection, XCB_COPY_FROM_PARENT, data->window, screen->root, x, y, w, h, 0, XCB_WINDOW_CLASS_INPUT_OUTPUT, screen->root_visual, XCB_CW_EVENT_MASK, values);

    if(data->window == 0)
    {
        CF_FATAL("Failed to create window!");
        return FALSE;
    }

    xcb_map_window(data->connection, data->window);
    xcb_flush(data->connection);

    return TRUE;
}

void platformDestroyWindow(PlatformWindow* win)
{
    WindowData* data = (WindowData*)win->windowData;

    xcb_destroy_window(data->connection, data->window);
    xcb_disconnect(data->connection);

    cfFree(win->windowData);
}

void platformWriteConsole(u32 color, const char* msg)
{
    const char* colorCodes[] =
    {
        "[41;30m",
        "[0;31m",
        "[0;33m",
        "[0;37m",
        "[0;34m",
        "[0;32m",
    };

    printf("\e%s%s\e[0m", colorCodes[color], msg);
}

f64 platformGetTime()
{
    struct timespec time;

    clock_gettime(CLOCK_REALTIME, &time);

    return (f64)time.tv_sec + time.tv_nsec * 0.000000001;
}

#endif