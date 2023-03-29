#include "platform.h"

#ifdef PLATFORM_LINUX

#include <xcb/xcb.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>

#include "core/cf_memory.h"
#include "core/logger.h"
#include "core/event.h"

typedef struct WindowData
{
    xcb_connection_t* connection;
    xcb_window_t window;
    xcb_intern_atom_reply_t* closeReply;
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

    xcb_intern_atom_cookie_t protocolsCookie = xcb_intern_atom(data->connection, TRUE, 12, "WM_PROTOCOLS");
    xcb_intern_atom_reply_t* protocolsReply = xcb_intern_atom_reply(data->connection, protocolsCookie, 0);
    xcb_intern_atom_cookie_t closeCookie = xcb_intern_atom(data->connection, FALSE, 16, "WM_DELETE_WINDOW");
    data->closeReply = xcb_intern_atom_reply(data->connection, closeCookie, 0);
    xcb_change_property(data->connection, XCB_PROP_MODE_REPLACE, data->window, (*protocolsReply).atom, 4, 32, 1, &(*data->closeReply).atom);
    cfFree(protocolsReply);

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

void platformWindowUpdate(PlatformWindow* win)
{
    WindowData* data = (WindowData*)win->windowData;
    
    xcb_generic_event_t* e;

    while((e = xcb_poll_for_event(data->connection)))
    {
        switch(e->response_type & ~0x80)
        {
            case XCB_EXPOSE:
                break;
            
            case XCB_CLIENT_MESSAGE:
            {
                if((*(xcb_client_message_event_t*)e).data.data32[0] == (*data->closeReply).atom)
                {
                    fireEvent(EVENT_CODE_WINDOW_CLOSED, PNULL);
                    return;
                }
            }

            case XCB_CONFIGURE_NOTIFY:
            {
                const xcb_configure_notify_event_t* cfgEvent = (const xcb_configure_notify_event_t*)e;

                u32 windowSize[2];
                windowSize[0] = cfgEvent->width;
                windowSize[1] = cfgEvent->height;

                fireEvent(EVENT_CODE_WINDOW_RESIZED, windowSize);
            } break;

            default:
                break;
        }

        cfFree(e);
    }
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

#include <vulkan/vulkan_xcb.h>

const char* getPlatformSurfaceExtenion()
{
    return VK_KHR_XCB_SURFACE_EXTENSION_NAME;
}

VkSurfaceKHR createSurface(PlatformWindow* win, VkInstance instance, VkAllocationCallbacks* allocator)
{
    WindowData* data = (WindowData*)win->windowData;

    VkXcbSurfaceCreateInfoKHR surfaceInfo = {VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR};
    surfaceInfo.connection = data.connection;
    surfaceInfo.window = data.window;

    VkSurfaceKHR surface;
    if((vkCreateXcbSurfaceKHR(instance, &surfaceInfo, allocator, &surface) != VK_SUCCESS))
    {
        return 0;
    }

    return surface;
}

#endif