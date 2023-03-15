#include "platform/platform_window.h"
#include "platform/platform_check.h"
#include "core/event.h"

#ifdef CF_PLATFORM_WINDOWS

#include <Windows.h>
#include <stdlib.h>

static b8 hasClosed;

typedef struct windowData
{
    HINSTANCE instance;
    HWND handle;
} windowData;

LRESULT CALLBACK windowCallback(HWND hwnd, u32 msg, WPARAM wParam, LPARAM lParam);

static u32 w;
static u32 h;

b8 platformCreateWindow(PlatformWindow* win, u32 x, u32 y, u32 width, u32 height, const char* title)
{
    win->windowData = malloc(sizeof(windowData));
    windowData* data = (windowData*)win->windowData;

    data->instance = GetModuleHandleA(0);

    WNDCLASSA wc = {};
    wc.style = CS_DBLCLKS;
    wc.lpfnWndProc = windowCallback;
    wc.hInstance = data->instance;
    wc.hIcon = LoadIcon(data->instance, IDI_APPLICATION);
    wc.hCursor = LoadCursor(data->instance, IDC_ARROW);
    wc.hbrBackground = NULL;
    wc.lpszClassName = "crayfish_window_class";

    if(!RegisterClassA(&wc))
    {
        MessageBoxA(0, "Failed to register window class!", "Window error", MB_ICONERROR | MB_OK);
        return FALSE;
    }

    u32 clientX = x;
    u32 clientY = y;
    u32 clientWidth = width;
    u32 clientHeight = height;

    u32 windowX = clientX;
    u32 windowY = clientY;
    u32 windowWidth = clientWidth;
    u32 windowHeight = clientHeight;

    u32 winStyle = WS_OVERLAPPEDWINDOW;
    u32 exWinStyle = WS_EX_APPWINDOW;

    RECT border = {0, 0, 0, 0};
    AdjustWindowRectEx(&border, winStyle, 0, exWinStyle);

    windowX += border.left;
    windowY += border.top;
    windowWidth += border.right - border.left;
    windowHeight += border.bottom - border.top;

    HWND handle = CreateWindowExA(
        exWinStyle,
        "crayfish_window_class",
        title,
        winStyle,
        windowX,
        windowY,
        windowWidth,
        windowHeight,
        0, 0,
        data->instance,
        NULL
    );

    if(handle != 0)
    {
        data->handle = handle;
    }
    else
    {
        MessageBoxA(0, "Failed to create window!", "Window error", MB_ICONERROR | MB_OK);
        return FALSE;
    }

    i32 cmdShow = SW_SHOW;
    ShowWindow(data->handle, cmdShow);

    hasClosed = FALSE;
    w = clientWidth;
    h = clientHeight;

    return TRUE;
}

void platformDestroyWindow(PlatformWindow* win)
{
    windowData* data = (windowData*)win->windowData;

    DestroyWindow(data->handle);
    free(win->windowData);
}

void platformWindowUpdate(PlatformWindow* win)
{
    windowData* data = (windowData*)win->windowData;

    MSG msg;

    while(PeekMessageA(&msg, data->handle, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }

    win->hasClosed = hasClosed;
}

void getWindowSize(u32* width, u32* height)
{
    *width = w;
    *height = h;
}

LRESULT CALLBACK windowCallback(HWND hwnd, u32 msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
        case WM_ERASEBKGND:
            return 1;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        
        case WM_CLOSE:
            hasClosed = TRUE;
            return 0;
        
        case WM_SIZE:
        {
            RECT r = {0, 0, 0, 0};
            GetClientRect(hwnd, &r);

            w = r.right - r.left;
            h = r.bottom - r.top;
            fireEvent(EVENT_TYPE_WINDOW_RESIZED);
        } break;

        default:
            break;
    }

    return DefWindowProcA(hwnd, msg, wParam, lParam);
}

#endif