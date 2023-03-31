#ifndef VULKAN_SWAPCHAIN_H
#define VULKAN_SWAPCHAIN_H

#include "vulkan_def.h"

void createSwapchain(VulkanContext* context, VkSwapchainKHR oldSwapchain);
void createImageViews(VulkanContext* context);

#endif