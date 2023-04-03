#include "vulkan_command_buffers.h"

void createCommandPool(VulkanContext* context)
{
    VkCommandPoolCreateInfo graphicsQueueCommandPoolInfo = {VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO};
    graphicsQueueCommandPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    graphicsQueueCommandPoolInfo.queueFamilyIndex = context->graphicsQueueIndex;

    VkCommandPoolCreateInfo transferQueueCommandPoolInfo = {VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO};
    transferQueueCommandPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    transferQueueCommandPoolInfo.queueFamilyIndex = context->transferQueueIndex;

    VK_CHECK(vkCreateCommandPool(context->device, &graphicsQueueCommandPoolInfo, context->allocator, &context->graphicsQueueCommandPool));
    VK_CHECK(vkCreateCommandPool(context->device, &transferQueueCommandPoolInfo, context->allocator, &context->transferQueueCommandPool));
}

void allocateCommandBuffer(VulkanContext* context)
{
    VkCommandBufferAllocateInfo graphicsCmdAllocInfo = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
    graphicsCmdAllocInfo.commandPool = context->graphicsQueueCommandPool;
    graphicsCmdAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    graphicsCmdAllocInfo.commandBufferCount = 1;

    VkCommandBufferAllocateInfo transferCmdAllocInfo = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
    transferCmdAllocInfo.commandPool = context->transferQueueCommandPool;
    transferCmdAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    transferCmdAllocInfo.commandBufferCount = 1;

    VK_CHECK(vkAllocateCommandBuffers(context->device, &graphicsCmdAllocInfo, &context->graphicsQueueCommandBuffer));
    VK_CHECK(vkAllocateCommandBuffers(context->device, &transferCmdAllocInfo, &context->transferQueueCommandBuffer));
}