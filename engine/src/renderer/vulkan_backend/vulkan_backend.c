#include "vulkan_backend.h"

#include "core/logger.h"
#include "vulkan_def.h"
#include "vulkan_device.h"

static VulkanContext context;

b8 vulkan_backendStartup(PlatformWindow* win)
{
    CF_INFO("Vulkan Renderer started.");

    createDevice(&context, win);

    return TRUE;
}

void vulkan_backendShutdown()
{
    destroyDevice(&context);
    CF_INFO("Vulkan Renderer shut down.");
}

void vulkan_backendDrawFrame()
{
    
}

void vulkan_backendOnResize(u32 newWidth, u32 newHeight)
{

}