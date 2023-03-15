#include "platform/platform_window.h"
#include "platform/platform_check.h"
#include "core/logger.h"
#include "core/event.h"

#ifdef CF_PLATFORM_LINUX

#include <xcb/xcb.h>
#include <stdlib.h>

typedef struct windowData
{
    xcb_connection_t* connection;
    xcb_window_t window;

    xcb_intern_atom_reply_t* closeReply;
} windowData;

static u32 w;
static u32 h;

b8 platformCreateWindow(PlatformWindow* win, u32 x, u32 y, u32 width, u32 height, const char* title)
{
    win->windowData = malloc(sizeof(windowData));
    windowData* data = (windowData*)win->windowData;

    data->connection = xcb_connect(NULL, NULL);

    const xcb_setup_t* setup = xcb_get_setup(data->connection);
    xcb_screen_iterator_t iter = xcb_setup_roots_iterator(setup);
    xcb_screen_t* screen = iter.data;

    
    u32 eventMask = XCB_EVENT_MASK_STRUCTURE_NOTIFY;

    xcb_window_t handle = xcb_generate_id(data->connection);
    xcb_create_window(
        data->connection,
        XCB_COPY_FROM_PARENT,
        handle,
        screen->root,
        x, x,
        width, height,
        0,
        XCB_WINDOW_CLASS_INPUT_OUTPUT,
        screen->root_visual,
        XCB_CW_EVENT_MASK,
        &eventMask
    );

    if(handle != 0)
    {
        data->window = handle;
    }
    else
    {
        CF_FATAL("Failed to create window");
        return FALSE;
    }

    xcb_intern_atom_cookie_t protocolsCookie = xcb_intern_atom_unchecked(data->connection, 1, 12, "WM_PROTOCOLS");
    xcb_intern_atom_reply_t* protocolsReply = xcb_intern_atom_reply(data->connection, protocolsCookie, 0);
    xcb_intern_atom_cookie_t closeCookie = xcb_intern_atom_unchecked(data->connection, 0, 16, "WM_DELETE_WINDOW");
    data->closeReply = xcb_intern_atom_reply(data->connection, closeCookie, 0);
    xcb_change_property(data->connection, XCB_PROP_MODE_REPLACE, data->window, (*protocolsReply).atom, 4, 32, 1, &(data->closeReply->atom));

    xcb_map_window(data->connection, data->window);
    xcb_flush(data->connection);

    win->hasClosed = FALSE;
    w = width;
    h = height;

    return TRUE;
}

void platformDestroyWindow(PlatformWindow* win)
{
    windowData* data = (windowData*)win->windowData;

    xcb_destroy_window(data->connection, data->window);
    xcb_disconnect(data->connection);

    free(win->windowData);
}

void platformWindowUpdate(PlatformWindow* win)
{
    windowData* data = (windowData*)win->windowData;

    xcb_generic_event_t* e;
    while((e = xcb_poll_for_event(data->connection)))
    {
        switch(e->response_type & ~0x80)
        {
            case XCB_EXPOSE:
                xcb_flush(data->connection);
                break;
            
            case XCB_CLIENT_MESSAGE:
            {
                if((*(xcb_client_message_event_t*)e).data.data32[0] == (*data->closeReply).atom)
                    win->hasClosed = TRUE;
            } break;

            case XCB_CONFIGURE_NOTIFY:
            {
                const xcb_configure_notify_event_t* cfgEvent = (const xcb_configure_notify_event_t*)e;
                if(((cfgEvent->width != w) || (cfgEvent->height != h)))
                {
                    w = cfgEvent->width;
                    h = cfgEvent->height;

                    fireEvent(EVENT_TYPE_WINDOW_RESIZED);
                }
            } break;

            default:
                break;
        }

        free(e);
    }
}

void getWindowSize(u32* width, u32* height)
{
    *width = w;
    *height = h;
}

#endif