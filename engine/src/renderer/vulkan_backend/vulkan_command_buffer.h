#ifndef VULKAN_COMMAND_BUFFER_H
#define VULKAN_COMMAND_BUFFER_H

#include "vulkan_def.h"

b8 createCommandPool(VulkanContext* context);
void destroyCommandPool(VulkanContext* context);

b8 allocateCommandBuffer(VulkanContext* context);
void freeCommandBuffer(VulkanContext* context);

#endif