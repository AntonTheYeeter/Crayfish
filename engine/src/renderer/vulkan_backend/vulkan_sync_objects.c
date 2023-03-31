#include "vulkan_sync_objects.h"

void createSyncObjects(VulkanContext* context)
{
    VkSemaphoreCreateInfo semaInfo = {VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};

    VK_CHECK(vkCreateSemaphore(context->device, &semaInfo, context->allocator, &context->imageAvailableSemaphore));
    VK_CHECK(vkCreateSemaphore(context->device, &semaInfo, context->allocator, &context->renderFinishedSemaphore));

    VkFenceCreateInfo fenceInfo = {VK_STRUCTURE_TYPE_FENCE_CREATE_INFO};
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    VK_CHECK(vkCreateFence(context->device, &fenceInfo, context->allocator, &context->inFlightFence));
}