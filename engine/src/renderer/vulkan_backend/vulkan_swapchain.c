#include "vulkan_swapchain.h"
#include "vulkan_framebuffers.h"

#include <stdlib.h>

static VkSurfaceFormatKHR chooseSwapchainFormat(VkPhysicalDevice gpu, VkSurfaceKHR surface)
{
    u32 formatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(gpu, surface, &formatCount, NULL);
    VkSurfaceFormatKHR formats[formatCount];
    vkGetPhysicalDeviceSurfaceFormatsKHR(gpu, surface, &formatCount, formats);

    for(u32 i = 0; i < formatCount; i++)
    {
        if(formats[i].format == VK_FORMAT_B8G8R8A8_UNORM)
        {
            return formats[i];
        }
    }

    return formats[0];
}

static VkPresentModeKHR chooseSwapchainPresentMode(VkPhysicalDevice gpu, VkSurfaceKHR surface)
{
    u32 presentModeCount = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(gpu, surface, &presentModeCount, NULL);
    VkPresentModeKHR presentModes[presentModeCount];
    vkGetPhysicalDeviceSurfacePresentModesKHR(gpu, surface, &presentModeCount, presentModes);
    
    for(u32 i = 0; i < presentModeCount; i++)
    {
        if(presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            return presentModes[i];
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

b8 createSwapchain(VulkanSwapchain* swapchain, VulkanContext* context)
{
    VkSurfaceCapabilitiesKHR surfaceCaps;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(context->gpu, context->surface, &surfaceCaps);

    u32 imageCount = surfaceCaps.minImageCount;
    imageCount = (imageCount < surfaceCaps.maxImageCount) ? surfaceCaps.minImageCount + 1 : surfaceCaps.minImageCount;

    VkSurfaceFormatKHR format = chooseSwapchainFormat(context->gpu, context->surface);
    swapchain->imageFormat = format.format;

    VkSwapchainCreateInfoKHR swapchainInfo = {VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR};
    swapchainInfo.surface = context->surface;
    swapchainInfo.minImageCount = imageCount;
    swapchainInfo.imageFormat = format.format;
    swapchainInfo.imageColorSpace = format.colorSpace;
    swapchainInfo.imageExtent = surfaceCaps.currentExtent;
    swapchainInfo.imageArrayLayers = 1;
    swapchainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    
    u32 queueFamilies[] = {context->graphicsQueue.familyIndex, context->presentQueue.familyIndex};
    swapchainInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    swapchainInfo.queueFamilyIndexCount = 2;
    swapchainInfo.pQueueFamilyIndices = queueFamilies;

    swapchainInfo.preTransform = surfaceCaps.currentTransform;
    swapchainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchainInfo.presentMode = chooseSwapchainPresentMode(context->gpu, context->surface);
    swapchainInfo.clipped = VK_TRUE;
    swapchainInfo.oldSwapchain = VK_NULL_HANDLE;

    VK_CHECK(vkCreateSwapchainKHR(context->device, &swapchainInfo, context->allocator, &swapchain->handle));
    CF_DEBUG("Vulkan Swapchain created successfully.");

    VK_CHECK(vkGetSwapchainImagesKHR(context->device, swapchain->handle, &swapchain->imageCount, NULL));
    swapchain->images = calloc(swapchain->imageCount, sizeof(VkImage));
    VK_CHECK(vkGetSwapchainImagesKHR(context->device, swapchain->handle, &swapchain->imageCount, swapchain->images));

    VkImageViewCreateInfo imageViewInfo = {VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
    imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    imageViewInfo.format = format.format;
    imageViewInfo.components.r = VK_COMPONENT_SWIZZLE_R;
    imageViewInfo.components.g = VK_COMPONENT_SWIZZLE_G;
    imageViewInfo.components.b = VK_COMPONENT_SWIZZLE_B;
    imageViewInfo.components.a = VK_COMPONENT_SWIZZLE_A;
    imageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imageViewInfo.subresourceRange.baseMipLevel = 0;
    imageViewInfo.subresourceRange.levelCount = 1;
    imageViewInfo.subresourceRange.baseArrayLayer = 0;
    imageViewInfo.subresourceRange.layerCount = 1;

    swapchain->imageViews = calloc(swapchain->imageCount, sizeof(VkImageView));

    for(u32 i = 0; i < swapchain->imageCount; i++)
    {
        imageViewInfo.image = swapchain->images[i];
        VK_CHECK(vkCreateImageView(context->device, &imageViewInfo, context->allocator, &swapchain->imageViews[i]));
    }

    CF_DEBUG("Vulkan Image Views created successfully.");

    return TRUE;
}

void destroySwapchain(VulkanContext* context, VulkanSwapchain* swapchain)
{
    for(u32 i = 0; i < swapchain->imageCount; i++)
    {
        vkDestroyImageView(context->device, swapchain->imageViews[i], context->allocator);
    }

    vkDestroySwapchainKHR(context->device, swapchain->handle, context->allocator);

    free(swapchain->imageViews);
    free(swapchain->images);
}

b8 recreateSwapchain(VulkanSwapchain* swapchain, VulkanContext* context)
{
    vkDeviceWaitIdle(context->device);

    destroySwapchain(context, swapchain);
    destroyFramebuffers(context);

    createSwapchain(swapchain, context);
    createFramebuffers(context);

    return TRUE;
}