#include "vulkan_vertex_buffer.h"

#include "core/cf_memory.h"
#include "vulkan_buffer.h"

void createVertexBuffer(VulkanContext* context, Vertex* vertices, u32 numVertices)
{
    if((vertices == PNULL) || numVertices == 0)
    {
        CF_ERROR("No vertex data provided!");
        return;
    }

    u64 bufferSize = numVertices * sizeof(Vertex);

    // Staging Buffer
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    createVulkanBuffer(context, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer, &stagingBufferMemory);

    void* data;
    vkMapMemory(context->device, stagingBufferMemory, 0, bufferSize, 0, &data);
    cfCopyMemory(vertices, data, (u64)bufferSize);
    vkUnmapMemory(context->device, stagingBufferMemory);

    // Vertex Buffer

    createVulkanBuffer(context, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &context->vertexBuffer, &context->vertexBufferMemory);

    copyBuffer(context, stagingBuffer, context->vertexBuffer, bufferSize);

    vkDestroyBuffer(context->device, stagingBuffer, context->allocator);
    vkFreeMemory(context->device, stagingBufferMemory, context->allocator);

    context->numVertices = numVertices;
}