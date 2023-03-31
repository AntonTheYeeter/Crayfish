#include "vulkan_command_buffers.h"

void createCommandPool(VulkanContext* context)
{
    VkCommandPoolCreateInfo commandPoolInfo = {VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO};
    commandPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    commandPoolInfo.queueFamilyIndex = context->graphicsQueueIndex;

    VK_CHECK(vkCreateCommandPool(context->device, &commandPoolInfo, context->allocator, &context->commandPool));
}

void allocateCommandBuffer(VulkanContext* context)
{
    VkCommandBufferAllocateInfo cmdAllocInfo = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
    cmdAllocInfo.commandPool = context->commandPool;
    cmdAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    cmdAllocInfo.commandBufferCount = 1;

    VK_CHECK(vkAllocateCommandBuffers(context->device, &cmdAllocInfo, &context->commandBuffer));
}