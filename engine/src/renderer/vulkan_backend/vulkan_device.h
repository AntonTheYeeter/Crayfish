#ifndef VULKAN_DEVICE_H
#define VULKAN_DEVICE_H

#include "vulkan_def.h"

void createInstance(VulkanContext* context);
#ifdef _DEBUG
void createDebugMessenger(VulkanContext* context);
#endif
void selectGPU(VulkanContext* context);
void createDevice(VulkanContext* context);

#endif