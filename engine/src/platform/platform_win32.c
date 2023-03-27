#include "platform.h"

#ifdef PLATFORM_WINDOWS

#include <Windows.h>
#include <string.h>

#include "core/cf_memory.h"
#include "core/logger.h"
#include "core/event.h"

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

void platformWindowUpdate(PlatformWindow* win)
{
    MSG msg;

    while(PeekMessageA(&msg, 0, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }
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

f64 platformGetTime()
{
    LARGE_INTEGER freq;
    QueryPerformanceFrequency(&freq);

    f64 frequency = 1.0 / freq.QuadPart;

    LARGE_INTEGER time;
    QueryPerformanceCounter(&time);

    return (f64)time.QuadPart * frequency;
}

LRESULT CALLBACK windowCallabck(HWND hwnd, u32 msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
        case WM_ERASEBKGND:
            return 1;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        case WM_CLOSE:
            fireEvent(EVENT_CODE_WINDOW_CLOSED, PNULL);
            return 0;

        case WM_SIZE:
        {
            RECT r = {0, 0, 0, 0};
            GetClientRect(hwnd, &r);

            u32 windowSize[2];
            windowSize[0] = r.right - r.left;
            windowSize[1] = r.bottom - r.top;

            fireEvent(EVENT_CODE_WINDOW_RESIZED, windowSize);
        } break;

        default:
            break;
    }

    return DefWindowProcA(hwnd, msg, wParam, lParam);
}

// Vulkan platform functions

#include <vulkan/vulkan_win32.h>

const char* getPlatformSurfaceExtenion()
{
    return VK_KHR_WIN32_SURFACE_EXTENSION_NAME;
}

VkSurfaceKHR createSurface(PlatformWindow* win, VkInstance instance, VkAllocationCallbacks* allocator)
{
    WindowData* data = (WindowData*)win->windowData;

    VkWin32SurfaceCreateInfoKHR surfaceInfo = {VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR};
    surfaceInfo.hinstance = data->hInstance;
    surfaceInfo.hwnd = data->hwnd;

    VkSurfaceKHR surface;
    if((vkCreateWin32SurfaceKHR(instance, &surfaceInfo, allocator, &surface) != VK_SUCCESS))
    {
        return 0;
    }

    return surface;
}

#endif