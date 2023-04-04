#ifndef VULKAN_DEF_H
#define VULKAN_DEF_H

#include "def.h"

#include <vulkan/vulkan.h>

#include "core/logger.h"
#include "vulkan_utils.h"

typedef struct VulkanContext
{
    VkExtent2D windowExtent;
    VkAllocationCallbacks* allocator;

    VkInstance instance;
    VkDebugUtilsMessengerEXT debugMessenger;
    VkSurfaceKHR surface;
    VkPhysicalDevice gpu;

    u32 graphicsQueueIndex;
    VkQueue graphicsQueue;
    u32 presentQueueIndex;
    VkQueue presentQueue;
    u32 transferQueueIndex;
    VkQueue transferQueue;

    VkDevice device;
    VkSurfaceFormatKHR scFormat;
    VkSwapchainKHR swapchain;
    u32 scImageCount;
    VkImage* scImages;
    VkImageView* scImageViews;

    VkRenderPass renderPass;

    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;

    VkFramebuffer* scFramebuffers;

    VkCommandPool graphicsQueueCommandPool;
    VkCommandBuffer graphicsQueueCommandBuffer;

    VkCommandPool transferQueueCommandPool;
    VkCommandBuffer transferQueueCommandBuffer;

    VkSemaphore imageAvailableSemaphore;
    VkSemaphore renderFinishedSemaphore;
    VkFence inFlightFence;

    u32 numVertices;
    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;

    u32 numIndices;
    VkBuffer indexBuffer;
    VkDeviceMemory indexBufferMemory;
} VulkanContext;

#define VK_CHECK(res)                                                                                                                   \
{                                                                                                                                       \
    VkResult result = res;                                                                                                              \
    if(vulkanResultIsError(result)){CF_FATAL("Vulkan Error at %s line %i: %s", __FILE__, __LINE__, vulkanResultToString(result));}      \
}

#endif