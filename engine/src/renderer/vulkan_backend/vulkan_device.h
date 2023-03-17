#ifndef VULKAN_DEVICE_H
#define VULKAN_DEVICE_H

#include "vulkan_def.h"
#include "platform/platform.h"

b8 createDevice(VulkanContext* context, PlatformWindow* win);
void destroyDevice(VulkanContext* context);

#endif