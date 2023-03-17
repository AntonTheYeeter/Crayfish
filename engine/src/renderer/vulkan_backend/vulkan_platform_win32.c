#include "vulkan_platform.h"

#ifdef CF_PLATFORM_WINDOWS

#include "platform/win32/platform_win32.h"

#include <vulkan/vulkan_win32.h>

void createSurface(PlatformWindow* win, VulkanContext* context)
{
    win32_windowData* data = (win32_windowData*)win->windowData;

    VkWin32SurfaceCreateInfoKHR surfaceInfo = {VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR};
    surfaceInfo.hinstance = data->instance;
    surfaceInfo.hwnd = data->handle;

    VK_CHECK(vkCreateWin32SurfaceKHR(context->instance, &surfaceInfo, context->allocator, &context->surface));
    CF_DEBUG("Vulkan Surface created successfully.");
}

#endif