#include "vulkan_swapchain.h"

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

    if((surfaceCaps.maxImageCount > 0) && (imageCount > surfaceCaps.maxImageCount))
    {
        imageCount = surfaceCaps.maxImageCount;
    }

    VkSurfaceFormatKHR format = chooseSwapchainFormat(context->gpu, context->surface);

    VkSwapchainCreateInfoKHR swapchainInfo = {VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR};
    swapchainInfo.surface = context->surface;
    swapchainInfo.minImageCount = imageCount;
    swapchainInfo.imageFormat = format.format;
    swapchainInfo.imageColorSpace = format.colorSpace;
    swapchainInfo.imageExtent = surfaceCaps.currentExtent;
    swapchainInfo.imageArrayLayers = 1;
    swapchainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    if(context->graphicsQueue.familyIndex != context->presentQueue.familyIndex)
    {
        u32 queueFamilies[] = {context->graphicsQueue.familyIndex, context->presentQueue.familyIndex};
        swapchainInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapchainInfo.queueFamilyIndexCount = 2;
        swapchainInfo.pQueueFamilyIndices = queueFamilies;
    }
    else
    {
        swapchainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    swapchainInfo.preTransform = surfaceCaps.currentTransform;
    swapchainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchainInfo.presentMode = chooseSwapchainPresentMode(context->gpu, context->surface);
    swapchainInfo.clipped = VK_TRUE;
    swapchainInfo.oldSwapchain = VK_NULL_HANDLE;

    VK_CHECK(vkCreateSwapchainKHR(context->device, &swapchainInfo, context->allocator, &swapchain->handle));
    CF_DEBUG("Vulkan Swapchain created successfully.");

    u32 swapchainImageCount = 0;
    vkGetSwapchainImagesKHR(context->device, swapchain->handle, &swapchainImageCount, NULL);
    VkImage swapchainImages[swapchainImageCount];
    vkGetSwapchainImagesKHR(context->device, swapchain->handle, &swapchainImageCount, swapchainImages);

    swapchain->images = swapchainImages;

    return TRUE;
}

void destroySwapchain(VulkanContext* context, VulkanSwapchain* swapchain)
{
    vkDestroySwapchainKHR(context->device, swapchain->handle, context->allocator);
}

b8 recreateSwapchain(VulkanSwapchain* swapchain, VulkanContext* context)
{
    return TRUE;
}