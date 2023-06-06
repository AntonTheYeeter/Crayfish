#include "vulkan_buffer.h"

static u32 findMemoryType(VkPhysicalDevice gpu, u32 typeFilter, VkMemoryPropertyFlags props)
{
    VkPhysicalDeviceMemoryProperties memProps;
    vkGetPhysicalDeviceMemoryProperties(gpu, &memProps);

    for(u32 i = 0; i < memProps.memoryTypeCount; i++)
    {
        if((typeFilter & (1 << i)) && (memProps.memoryTypes[i].propertyFlags & props) == props)
        {
            return i;
        }
    }

    CF_FATAL("No suitable memory type found!");
    return 0;
}

void createVulkanBuffer(VulkanContext* context, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memProps, VkBuffer* buffer, VkDeviceMemory* memory)
{
    VkBufferCreateInfo bufferInfo = {VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_CONCURRENT;
    bufferInfo.queueFamilyIndexCount = 2;
    u32 queueFamilies[] = {context->graphicsQueueIndex, context->transferQueueIndex};
    bufferInfo.pQueueFamilyIndices = queueFamilies;

    VK_CHECK(vkCreateBuffer(context->device, &bufferInfo, context->allocator, buffer));

    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(context->device, *buffer, &memoryRequirements);

    VkMemoryAllocateInfo allocInfo = {VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};
    allocInfo.allocationSize = memoryRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(context->gpu, memoryRequirements.memoryTypeBits, memProps);

    VK_CHECK(vkAllocateMemory(context->device, &allocInfo, context->allocator, memory));

    vkBindBufferMemory(context->device, *buffer, *memory, 0);
}

void copyBuffer(VulkanContext* context, VkBuffer src, VkBuffer dst, VkDeviceSize size)
{
    VkCommandBufferBeginInfo cmdBeginInfo = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
    cmdBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    VK_CHECK(vkBeginCommandBuffer(context->transferQueueCommandBuffer, &cmdBeginInfo));

    VkBufferCopy copyRegion = {0};
    copyRegion.size = size;

    vkCmdCopyBuffer(context->transferQueueCommandBuffer, src, dst, 1, &copyRegion);

    VK_CHECK(vkEndCommandBuffer(context->transferQueueCommandBuffer));

    VkSubmitInfo submitInfo = {VK_STRUCTURE_TYPE_SUBMIT_INFO};
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &context->transferQueueCommandBuffer;

    VK_CHECK(vkQueueSubmit(context->transferQueue, 1, &submitInfo, VK_NULL_HANDLE));
    VK_CHECK(vkQueueWaitIdle(context->transferQueue));
}