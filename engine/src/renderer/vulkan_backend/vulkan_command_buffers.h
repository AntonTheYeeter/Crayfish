#ifndef VULKAN_COMMAND_BUFFERS_H
#define VULKAN_COMMAND_BUFFERS_H

#include "vulkan_def.h"

void createCommandPool(VulkanContext* context);
void allocateCommandBuffer(VulkanContext* context);

#endif