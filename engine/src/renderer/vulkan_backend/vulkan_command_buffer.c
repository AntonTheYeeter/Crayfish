#include "vulkan_command_buffer.h"

b8 createCommandPool(VulkanContext* context)
{
    VkCommandPoolCreateInfo commandPoolInfo = {VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO};
    commandPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    commandPoolInfo.queueFamilyIndex = context->graphicsQueue.familyIndex;

    VK_CHECK(vkCreateCommandPool(context->device, &commandPoolInfo, context->allocator, &context->commandPool));

    return TRUE;
}

void destroyCommandPool(VulkanContext* context)
{
    vkDestroyCommandPool(context->device, context->commandPool, context->allocator);
}

b8 allocateCommandBuffer(VulkanContext* context)
{
    VkCommandBufferAllocateInfo allocInfo = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
    allocInfo.commandPool = context->commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;

    VK_CHECK(vkAllocateCommandBuffers(context->device, &allocInfo, &context->commandBuffer));

    return TRUE;
}

void freeCommandBuffer(VulkanContext* context)
{
    vkFreeCommandBuffers(context->device, context->commandPool, 1, &context->commandBuffer);
}