#include "vulkan_framebuffers.h"

#include <stdlib.h>

b8 createFramebuffers(VulkanContext* context)
{
    context->scFramebuffers = calloc(context->swapchain.imageCount, sizeof(VkFramebuffer));

    VkFramebufferCreateInfo framebufferInfo = {VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO};
    framebufferInfo.renderPass = context->renderPass;
    framebufferInfo.width = context->windowExtent.width;
    framebufferInfo.height = context->windowExtent.height;
    framebufferInfo.layers = 1;

    for(u32 i = 0; i < context->swapchain.imageCount; i++)
    {
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = &context->swapchain.imageViews[i];

        VK_CHECK(vkCreateFramebuffer(context->device, &framebufferInfo, context->allocator, &context->scFramebuffers[i]));
    }

    CF_DEBUG("Vulkan Framebuffers created successfully.");

    return TRUE;
}

void destroyFramebuffers(VulkanContext* context)
{
    for(u32 i = 0; i < context->swapchain.imageCount; i++)
    {
        vkDestroyFramebuffer(context->device, context->scFramebuffers[i], context->allocator);
    }
}