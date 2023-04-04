#include "vulkan_index_buffer.h"

#include "vulkan_buffer.h"
#include "core/cf_memory.h"

void createIndexBuffer(VulkanContext* context, u32* indices, u32 numIndices)
{
    VkDeviceSize bufferSize = numIndices * sizeof(u32);

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    createVulkanBuffer(context, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer, &stagingBufferMemory);

    void* data;
    vkMapMemory(context->device, stagingBufferMemory, 0, bufferSize, 0, &data);
    cfCopyMemory(indices, data, bufferSize);
    vkUnmapMemory(context->device, stagingBufferMemory);

    createVulkanBuffer(context, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &context->indexBuffer, &context->indexBufferMemory);

    copyBuffer(context, stagingBuffer, context->indexBuffer, bufferSize);

    vkDestroyBuffer(context->device, stagingBuffer, context->allocator);
    vkFreeMemory(context->device, stagingBufferMemory, context->allocator);
}