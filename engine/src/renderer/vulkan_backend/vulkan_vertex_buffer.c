#include "vulkan_vertex_buffer.h"

#include "core/cf_memory.h"

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

static void copyBuffer(VulkanContext* context, VkBuffer src, VkBuffer dst, VkDeviceSize size)
{
    VkCommandBufferBeginInfo cmdBeginInfo = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
    cmdBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    VK_CHECK(vkBeginCommandBuffer(context->transferQueueCommandBuffer, &cmdBeginInfo));

    VkBufferCopy copyRegion = {};
    copyRegion.size = size;

    vkCmdCopyBuffer(context->transferQueueCommandBuffer, src, dst, 1, &copyRegion);

    VK_CHECK(vkEndCommandBuffer(context->transferQueueCommandBuffer));

    VkSubmitInfo submitInfo = {VK_STRUCTURE_TYPE_SUBMIT_INFO};
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &context->transferQueueCommandBuffer;

    VK_CHECK(vkQueueSubmit(context->transferQueue, 1, &submitInfo, VK_NULL_HANDLE));
    VK_CHECK(vkQueueWaitIdle(context->transferQueue));
}

void createVertexBuffer(VulkanContext* context, Vertex* vertices, u32 numVertices)
{
    if((vertices == PNULL) || numVertices == 0)
    {
        CF_ERROR("No vertex data provided!");
        return;
    }

    u64 bufferSize = numVertices * sizeof(Vertex);

    // Staging Buffer

    VkBufferCreateInfo sbInfo = {VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
    sbInfo.size = bufferSize;
    sbInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    sbInfo.sharingMode = VK_SHARING_MODE_CONCURRENT;
    sbInfo.queueFamilyIndexCount = 2;
    u32 queueFamilies[] = {context->graphicsQueueIndex, context->transferQueueIndex};
    sbInfo.pQueueFamilyIndices = queueFamilies;

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    VK_CHECK(vkCreateBuffer(context->device, &sbInfo, context->allocator, &stagingBuffer));

    VkMemoryRequirements sbMemoryRequirements;
    vkGetBufferMemoryRequirements(context->device, stagingBuffer, &sbMemoryRequirements);

    VkMemoryAllocateInfo sbMemAllocInfo = {VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};
    sbMemAllocInfo.allocationSize = sbMemoryRequirements.size;
    sbMemAllocInfo.memoryTypeIndex = findMemoryType(context->gpu, sbMemoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    VK_CHECK(vkAllocateMemory(context->device, &sbMemAllocInfo, context->allocator, &stagingBufferMemory));

    vkBindBufferMemory(context->device, stagingBuffer, stagingBufferMemory, 0);

    void* data;
    vkMapMemory(context->device, stagingBufferMemory, 0, sbInfo.size, 0, &data);
    cfCopyMemory(vertices, data, (u64)sbInfo.size);
    vkUnmapMemory(context->device, stagingBufferMemory);

    // Vertex Buffer

    VkBufferCreateInfo vbInfo = {VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
    vbInfo.size = bufferSize;
    vbInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    vbInfo.sharingMode = VK_SHARING_MODE_CONCURRENT;
    vbInfo.queueFamilyIndexCount = 2;
    vbInfo.pQueueFamilyIndices = queueFamilies;

    VK_CHECK(vkCreateBuffer(context->device, &vbInfo, context->allocator, &context->vertexBuffer));

    VkMemoryRequirements vbMemoryRequirements;
    vkGetBufferMemoryRequirements(context->device, context->vertexBuffer, &vbMemoryRequirements);

    VkMemoryAllocateInfo vbMemAllocInfo = {VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};
    vbMemAllocInfo.allocationSize = vbMemoryRequirements.size;
    vbMemAllocInfo.memoryTypeIndex = findMemoryType(context->gpu, vbMemoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    VK_CHECK(vkAllocateMemory(context->device, &vbMemAllocInfo, context->allocator, &context->vertexBufferMemory));

    vkBindBufferMemory(context->device, context->vertexBuffer, context->vertexBufferMemory, 0);

    copyBuffer(context, stagingBuffer, context->vertexBuffer, bufferSize);

    vkDestroyBuffer(context->device, stagingBuffer, context->allocator);
    vkFreeMemory(context->device, stagingBufferMemory, context->allocator);

    context->numVertices = numVertices;
}