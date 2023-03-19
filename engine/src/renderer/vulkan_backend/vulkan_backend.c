#include "vulkan_backend.h"

#include "core/logger.h"
#include "core/assertion.h"
#include "vulkan_def.h"
#include "vulkan_device.h"
#include "vulkan_swapchain.h"

static VulkanContext context;

b8 vulkan_backendStartup(PlatformWindow* win)
{
    CF_INFO("Vulkan Renderer started.");

    CF_ASSERT(createDevice(&context, win));
    CF_ASSERT(createSwapchain(&context.swapchain, &context));

    return TRUE;
}

void vulkan_backendShutdown()
{
    destroySwapchain(&context, &context.swapchain);
    destroyDevice(&context);
    CF_INFO("Vulkan Renderer shut down.");
}

void vulkan_backendDrawFrame()
{
    
}

void vulkan_backendOnResize(u32 newWidth, u32 newHeight)
{

}