#include "vulkan_backend.h"

#include "core/logger.h"
#include "core/assertion.h"
#include "vulkan_def.h"
#include "vulkan_device.h"
#include "vulkan_swapchain.h"
#include "vulkan_render_pass.h"
#include "vulkan_framebuffers.h"
#include "vulkan_command_buffer.h"
#include "vulkan_sync_objects.h"

static VulkanContext context;

b8 vulkan_backendStartup(PlatformWindow* win)
{
    CF_INFO("Vulkan Renderer started.");

    getWindowSize(&context.windowExtent.width, &context.windowExtent.height);

    CF_ASSERT(createDevice(&context, win));
    CF_ASSERT(createSwapchain(&context.swapchain, &context));
    CF_ASSERT(createRenderPass(&context));
    CF_ASSERT(createFramebuffers(&context));
    CF_ASSERT(createCommandPool(&context));
    CF_ASSERT(allocateCommandBuffer(&context));
    CF_ASSERT(createSyncObjects(&context));

    return TRUE;
}

void vulkan_backendShutdown()
{
    destroySyncObjects(&context);
    freeCommandBuffer(&context);
    destroyCommandPool(&context);
    destroyFramebuffers(&context);
    destroyRenderPass(&context);
    destroySwapchain(&context, &context.swapchain);
    destroyDevice(&context);
    CF_INFO("Vulkan Renderer shut down.");
}

void vulkan_backendDrawFrame()
{
    vkWaitForFences(context.device, 1, &context.inFlightFence, VK_TRUE, U64_MAX);
    vkResetFences(context.device, 1, &context.inFlightFence);

    u32 imageIndex = 0;
    vkAcquireNextImageKHR(context.device, context.swapchain.handle, U64_MAX, context.imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

    vkResetCommandBuffer(context.commandBuffer, 0);

    VkCommandBufferBeginInfo cmdBeginInfo = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
    cmdBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    VK_CHECK(vkBeginCommandBuffer(context.commandBuffer, &cmdBeginInfo));


    VkRenderPassBeginInfo rpBeginInfo = {VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO};
    rpBeginInfo.renderPass = context.renderPass;
    rpBeginInfo.framebuffer = context.scFramebuffers[imageIndex];

    VkOffset2D offset = {0, 0};

    VkRect2D renderArea = {};
    renderArea.extent = context.windowExtent;
    renderArea.offset = offset;

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
    vkEndCommandBuffer(context.commandBuffer);

    VkSubmitInfo submitInfo = {VK_STRUCTURE_TYPE_SUBMIT_INFO};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &context.imageAvailableSemaphore;

    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &context.commandBuffer;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &context.renderFinishedSemaphore;

    VK_CHECK(vkQueueSubmit(context.graphicsQueue.handle, 1, &submitInfo, context.inFlightFence));

    VkPresentInfoKHR presentInfo = {VK_STRUCTURE_TYPE_PRESENT_INFO_KHR};
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &context.renderFinishedSemaphore;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &context.swapchain.handle;
    presentInfo.pImageIndices = &imageIndex;
    
    VK_CHECK(vkQueuePresentKHR(context.presentQueue.handle, &presentInfo));
}

void vulkan_backendOnResize(u32 newWidth, u32 newHeight)
{

}