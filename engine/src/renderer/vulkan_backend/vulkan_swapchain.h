#ifndef VULKAN_SWAPCHAIN_H
#define VULKAN_SWAPCHAIN_H

#include "vulkan_def.h"

b8 createSwapchain(VulkanSwapchain* swapchain, VulkanContext* context);
void destroySwapchain(VulkanContext* context, VulkanSwapchain* swapchain);
b8 recreateSwapchain(VulkanSwapchain* swapchain, VulkanContext* context);

#endif