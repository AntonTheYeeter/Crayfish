#ifndef VULKAN_BUFFER_H
#define VULKAN_BUFFER_H

#include "vulkan_def.h"

void createVulkanBuffer(VulkanContext* context, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memProps, VkBuffer* buffer, VkDeviceMemory* memory);
void copyBuffer(VulkanContext* context, VkBuffer src, VkBuffer dst, VkDeviceSize size);

#endif