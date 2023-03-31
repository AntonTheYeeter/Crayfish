#include "vulkan_render_pass.h"

void createRenderPass(VulkanContext* context)
{
    VkAttachmentDescription attachment = {};
    attachment.format = context->scFormat.format;
    attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachment = {};
    colorAttachment.attachment = 0;
    colorAttachment.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachment;

    VkRenderPassCreateInfo rpInfo = {VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO};
    rpInfo.attachmentCount = 1;
    rpInfo.pAttachments = &attachment;
    rpInfo.subpassCount = 1;
    rpInfo.pSubpasses = &subpass;
    rpInfo.dependencyCount = 0; // TODO:
    rpInfo.pDependencies = PNULL;// TODO:

    VK_CHECK(vkCreateRenderPass(context->device, &rpInfo, context->allocator, &context->renderPass));
}

void createFramebuffers(VulkanContext* context)
{
    VkFramebufferCreateInfo fbInfo = {VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO};
    fbInfo.renderPass = context->renderPass;
    fbInfo.width = context->windowExtent.width;
    fbInfo.height = context->windowExtent.height;
    fbInfo.layers = 1;

    context->scFramebuffers = cfAllocate(context->scImageCount * sizeof(VkFramebuffer));

    for(u32 i = 0; i < context->scImageCount; i++)
    {
        fbInfo.attachmentCount = 1;
        fbInfo.pAttachments = &context->scImageViews[i];

        VK_CHECK(vkCreateFramebuffer(context->device, &fbInfo, context->allocator, &context->scFramebuffers[i]));
    }
}
