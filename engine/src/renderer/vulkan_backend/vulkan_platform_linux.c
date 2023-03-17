#include "vulkan_platform.h"

#ifdef CF_PLATFORM_LINUX

#include "platform/linux/platform_linux.h"
#include "core/logger.h"

#include <vulkan/vulkan_xcb.h>

void createSurface(PlatformWindow* win, VulkanContext* context)
{
    linux_windowData* data = (linux_windowData*)win->windowData;

    VkXcbSurfaceCreateInfoKHR surfaceInfo = {VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR};
    surfaceInfo.connection = data->connection;
    surfaceInfo.window = data->window;

    VK_CHECK(vkCreateXcbSurfaceKHR(context->instance, &surfaceInfo, context->allocator, &context->surface));
    CF_DEBUG("Vulkan Surface created successfully.");
}

#endif