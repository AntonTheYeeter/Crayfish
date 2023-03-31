#include "vulkan_backend.h"

#include "vulkan_def.h"
#include "core/cf_memory.h"

#include "vulkan_device.h"
#include "vulkan_swapchain.h"
#include "vulkan_render_pass.h"
#include "vulkan_command_buffers.h"
#include "vulkan_sync_objects.h"

static VulkanContext context;

b8 vulkan_rendererBackendStartup(PlatformWindow* win, u32 windowWidth, u32 windowHeight)
{
    context.windowExtent.width = windowWidth;
    context.windowExtent.height = windowHeight;

    createInstance(&context);
    #ifdef _DEBUG
    createDebugMessenger(&context);
    #endif
    context.surface = createSurface(win, context.instance, context.allocator);
    selectGPU(&context);
    createDevice(&context);
    createSwapchain(&context, 0);
    createImageViews(&context);
    createRenderPass(&context);
    createFramebuffers(&context);
    createCommandPool(&context);
    allocateCommandBuffer(&context);
    createSyncObjects(&context);

    return TRUE;
}

void vulkan_rendererBackendShutdown()
{
    vkDeviceWaitIdle(context.device);

    vkDestroyFence(context.device, context.inFlightFence, context.allocator);
    vkDestroySemaphore(context.device, context.renderFinishedSemaphore, context.allocator);
    vkDestroySemaphore(context.device, context.imageAvailableSemaphore, context.allocator);

    vkFreeCommandBuffers(context.device, context.commandPool, 1, &context.commandBuffer);
    vkDestroyCommandPool(context.device, context.commandPool, context.allocator);

    for(u32 i = 0; i < context.scImageCount; i++)
    {
        vkDestroyFramebuffer(context.device, context.scFramebuffers[i], context.allocator);
    }

    vkDestroyRenderPass(context.device, context.renderPass, context.allocator);

    for(u32 i = 0; i < context.scImageCount; i++)
    {
        vkDestroyImageView(context.device, context.scImageViews[i], context.allocator);
    }
    cfFree(context.scImageViews);
    cfFree(context.scImages);

    vkDestroySwapchainKHR(context.device, context.swapchain, context.allocator);
    vkDestroyDevice(context.device, context.allocator);
    vkDestroySurfaceKHR(context.instance, context.surface, context.allocator);

    if(context.debugMessenger != VK_NULL_HANDLE)
    {
        PFN_vkDestroyDebugUtilsMessengerEXT destroyFunc = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(context.instance, "vkDestroyDebugUtilsMessengerEXT");

        destroyFunc(context.instance, context.debugMessenger, context.allocator);
    }

    vkDestroyInstance(context.instance, context.allocator);
}

void vulkan_rendererBackendDrawFrame(f32 delta)
{
    vkWaitForFences(context.device, 1, &context.inFlightFence, VK_TRUE, U64_MAX);
    vkResetFences(context.device, 1, &context.inFlightFence);

    u32 imageIndex = 0;
    VkResult res = vkAcquireNextImageKHR(context.device, context.swapchain, U64_MAX, context.imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

    if(res == VK_ERROR_OUT_OF_DATE_KHR)
    {
        return;
    }

    VK_CHECK(vkResetCommandBuffer(context.commandBuffer, 0));

    VkCommandBufferBeginInfo cmdBeginInfo = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};

    VK_CHECK(vkBeginCommandBuffer(context.commandBuffer, &cmdBeginInfo));

    VkRenderPassBeginInfo rpBeginInfo = {VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO};
    rpBeginInfo.renderPass = context.renderPass;
    rpBeginInfo.framebuffer = context.scFramebuffers[imageIndex];

    VkRect2D renderArea = {};
    renderArea.extent = context.windowExtent;

    rpBeginInfo.renderArea = renderArea;

    VkClearColorValue clearColor = {1.0f, 0.0f, 0.0f, 1.0f};

    VkClearValue clearValue = {};
    clearValue.color = clearColor;

    rpBeginInfo.clearValueCount = 1;
    rpBeginInfo.pClearValues = &clearValue;

    vkCmdBeginRenderPass(context.commandBuffer, &rpBeginInfo, 0);

    {

    }

    vkCmdEndRenderPass(context.commandBuffer);

    VK_CHECK(vkEndCommandBuffer(context.commandBuffer));

    VkSubmitInfo submitInfo = {VK_STRUCTURE_TYPE_SUBMIT_INFO};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &context.imageAvailableSemaphore;

    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &context.commandBuffer;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &context.renderFinishedSemaphore;

    VK_CHECK(vkQueueSubmit(context.graphicsQueue, 1, &submitInfo, context.inFlightFence));

    VkPresentInfoKHR presentInfo = {VK_STRUCTURE_TYPE_PRESENT_INFO_KHR};
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &context.renderFinishedSemaphore;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &context.swapchain;
    presentInfo.pImageIndices = &imageIndex;

    res = vkQueuePresentKHR(context.presentQueue, &presentInfo);

    if(res == VK_ERROR_OUT_OF_DATE_KHR || res == VK_SUBOPTIMAL_KHR)
    {
        return;
    }
}

void recreateSwapchain(u32 newWidth, u32 newHeight)
{
    vkDeviceWaitIdle(context.device);

    context.windowExtent.width = newWidth;
    context.windowExtent.height = newHeight;

    VkSwapchainKHR oldSwapchain = context.swapchain;
    createSwapchain(&context, oldSwapchain);

    for(u32 i = 0; i < context.scImageCount; i++)
    {
        vkDestroyFramebuffer(context.device, context.scFramebuffers[i], context.allocator);
    }

    for(u32 i = 0; i < context.scImageCount; i++)
    {
        vkDestroyImageView(context.device, context.scImageViews[i], context.allocator);
    }

    createImageViews(&context);
    createFramebuffers(&context);

    vkDestroySwapchainKHR(context.device, oldSwapchain, context.allocator);
}

void vulkan_rendererBackendOnResize(u32 newWidth, u32 newHeight)
{
    recreateSwapchain(newWidth, newHeight);
}