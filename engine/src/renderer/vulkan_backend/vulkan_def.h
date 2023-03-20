#ifndef VULKAN_DEF_H
#define VULKAN_DEF_H

#include "def.h"
#include "core/logger.h"
#include "vulkan_utils.h"

#include <vulkan/vulkan.h>

#define VK_CHECK(res)                                                       \
{                                                                           \
    VkResult result = res;                                                  \
    if(vulkanResultIsError(result))                                         \
    {                                                                       \
        CF_FATAL("Vulkan Error: %s\n", vulkanResultToString(result));       \
    }                                                                       \
}

typedef struct VulkanQueue
{
    VkQueue handle;
    u32 familyIndex;
} VulkanQueue;

typedef struct VulkanSwapchain
{
    VkFormat imageFormat;
    VkSwapchainKHR handle;
    u32 imageCount;
    VkImage* images;
    VkImageView* imageViews;
} VulkanSwapchain;

typedef struct VulkanContext
{
    VkExtent2D windowExtent;
    VkAllocationCallbacks* allocator;
    VkDebugUtilsMessengerEXT debugger;

    VkInstance instance;
    VkSurfaceKHR surface;
    VkPhysicalDevice gpu;
    VkDevice device;

    VulkanQueue graphicsQueue;
    VulkanQueue presentQueue;
    VulkanQueue transferQueue;

    VulkanSwapchain swapchain;

    VkRenderPass renderPass;
    VkFramebuffer* scFramebuffers;

    VkCommandPool commandPool;
    VkCommandBuffer commandBuffer;

    VkSemaphore imageAvailableSemaphore;
    VkSemaphore renderFinishedSemaphore;
    VkFence inFlightFence;
} VulkanContext;

#endif