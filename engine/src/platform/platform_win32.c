#include "platform.h"

#ifdef PLATFORM_WINDOWS

#include <Windows.h>
#include <string.h>

#include "core/cf_memory.h"
#include "core/logger.h"

typedef struct WindowData
{
    HINSTANCE hInstance;
    HWND hwnd;
} WindowData;

LRESULT CALLBACK windowCallabck(HWND hwnd, u32 msg, WPARAM wParam, LPARAM lParam);

b8 platformCreateWindow(PlatformWindow* win, u32 x, u32 y, u32 w, u32 h, const char* title)
{
    win->windowData = cfAllocate(sizeof(WindowData));
    WindowData* data = (WindowData*)win->windowData;

    data->hInstance = GetModuleHandleA(0);

    WNDCLASSA wc;
    cfZeroMemory(&wc, sizeof(WNDCLASSA));
    wc.style = CS_DBLCLKS;
    wc.lpfnWndProc = windowCallabck;
    wc.hInstance = data->hInstance;
    wc.hIcon = LoadIcon(data->hInstance, IDI_APPLICATION);
    wc.hCursor = LoadCursor(data->hInstance, IDC_ARROW);
    wc.hbrBackground = PNULL;
    wc.lpszClassName = "crayfish_window_class";

    if(!RegisterClassA(&wc))
    {
        CF_FATAL("Failed to register window class!");
        return FALSE;
    }

    u32 clientX = x;
    u32 clientY = y;
    u32 clientW = w;
    u32 clientH = h;

    u32 windowX = clientX;
    u32 windowY = clientY;
    u32 windowW = clientW;
    u32 windowH = clientH;

    u32 winStyle = WS_OVERLAPPEDWINDOW;
    u32 exWinStyle = WS_EX_APPWINDOW;

    RECT border = {0, 0, 0, 0};
    AdjustWindowRectEx(&border, winStyle, FALSE, exWinStyle);

    windowX += border.left;
    windowY += border.top;
    windowW += border.right - border.left;
    windowH += border.bottom - border.top;

    data->hwnd = CreateWindowExA(exWinStyle, "crayfish_window_class", title, winStyle, windowX, windowY, windowW, windowH, 0, 0, data->hInstance, 0);

    if(data->hwnd == 0)
    {
        CF_FATAL("Failed to create window!");
        return FALSE;
    }

    i32 cmdShow = SW_SHOW;
    ShowWindow(data->hwnd, cmdShow);

    return TRUE;
}

void platformDestroyWindow(PlatformWindow* win)
{
    WindowData* data = (WindowData*)win->windowData;

    DestroyWindow(data->hwnd);
    cfFree(win->windowData);
}

void platformWriteConsole(u32 color, const char* msg)
{
    HANDLE stdHandle = GetStdHandle(STD_OUTPUT_HANDLE);

    OutputDebugStringA(msg);

    u32 colors[] =
    {
        BACKGROUND_RED,
        FOREGROUND_RED,
        FOREGROUND_RED | FOREGROUND_GREEN, // Yellow
        FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE, // White
        FOREGROUND_BLUE,
        FOREGROUND_GREEN
    };

    SetConsoleTextAttribute(stdHandle, colors[color]);

    DWORD numWritten = 0;
    WriteConsoleA(stdHandle, msg, strlen(msg), &numWritten, PNULL);
}

u64 getMilliseconds()
{
    LARGE_INTEGER freq;
    QueryPerformanceFrequency(&freq);
    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);

    return (1000LL * now.QuadPart) / freq.QuadPart;
}

LRESULT CALLBACK windowCallabck(HWND hwnd, u32 msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
        default:
            break;
    }

    return DefWindowProcA(hwnd, msg, wParam, lParam);
}

#endif