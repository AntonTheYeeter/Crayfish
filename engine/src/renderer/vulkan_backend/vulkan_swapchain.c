#include "vulkan_swapchain.h"

#include "core/cf_memory.h"

void createSwapchain(VulkanContext* context, VkSwapchainKHR oldSwapchain)
{
    VkSurfaceCapabilitiesKHR surfaceCaps;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(context->gpu, context->surface, &surfaceCaps);

    VkSwapchainCreateInfoKHR scInfo = {VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR};
    scInfo.surface = context->surface;
    scInfo.minImageCount = surfaceCaps.minImageCount + 1 < surfaceCaps.maxImageCount ? surfaceCaps.minImageCount + 1 : surfaceCaps.minImageCount;

    {
        u32 formatCount = 0;
        vkGetPhysicalDeviceSurfaceFormatsKHR(context->gpu, context->surface, &formatCount, PNULL);
        VkSurfaceFormatKHR availableFormats[formatCount];
        vkGetPhysicalDeviceSurfaceFormatsKHR(context->gpu, context->surface, &formatCount, availableFormats);

        b8 foundFormat = FALSE;

        for(u32 i = 0; i < formatCount; i++)
        {
            if(availableFormats[i].format == VK_FORMAT_B8G8R8A8_UNORM && !foundFormat)
            {
                context->scFormat = availableFormats[i];
                foundFormat = TRUE;
                break;
            }
        }

        if(!foundFormat)
        {
            context->scFormat = availableFormats[0];
        }
    }

    scInfo.imageFormat = context->scFormat.format;
    scInfo.imageColorSpace = context->scFormat.colorSpace;
    scInfo.imageExtent = surfaceCaps.currentExtent;
    scInfo.imageArrayLayers = 1;
    scInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    scInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    scInfo.queueFamilyIndexCount = 2;

    u32 queueFamilies[] = {context->graphicsQueueIndex, context->presentQueueIndex};

    scInfo.pQueueFamilyIndices = queueFamilies;
    scInfo.preTransform = surfaceCaps.currentTransform;
    scInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

    VkPresentModeKHR presentMode;
    {
        u32 presentModeCount = 0;
        vkGetPhysicalDeviceSurfacePresentModesKHR(context->gpu, context->surface, &presentModeCount, PNULL);
        VkPresentModeKHR availablePresentModes[presentModeCount];
        vkGetPhysicalDeviceSurfacePresentModesKHR(context->gpu, context->surface, &presentModeCount, availablePresentModes);

        b8 foundPresentMode = FALSE;

        for(u32 i = 0; i < presentModeCount; i++)
        {
            if(availablePresentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
            {
                presentMode = availablePresentModes[i];
                foundPresentMode = TRUE;
                break;
            }
        }

        if(!foundPresentMode)
        {
            presentMode = VK_PRESENT_MODE_FIFO_KHR;
        }
    }

    scInfo.presentMode = presentMode;
    scInfo.clipped = VK_TRUE;
    scInfo.oldSwapchain = oldSwapchain;

    VK_CHECK(vkCreateSwapchainKHR(context->device, &scInfo, context->allocator, &context->swapchain));

    VK_CHECK(vkGetSwapchainImagesKHR(context->device, context->swapchain, &context->scImageCount, PNULL));
    context->scImages = cfAllocate(context->scImageCount * sizeof(VkImage));
    VK_CHECK(vkGetSwapchainImagesKHR(context->device, context->swapchain, &context->scImageCount, context->scImages));
}

void createImageViews(VulkanContext* context)
{
    VkImageViewCreateInfo imageViewInfo = {VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
    imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    imageViewInfo.format = context->scFormat.format;
    imageViewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imageViewInfo.subresourceRange.levelCount = 1;
    imageViewInfo.subresourceRange.layerCount = 1;

    context->scImageViews = cfAllocate(context->scImageCount * sizeof(VkImageView));

    for(u32 i = 0; i < context->scImageCount; i++)
    {
        imageViewInfo.image = context->scImages[i];
        VK_CHECK(vkCreateImageView(context->device, &imageViewInfo, context->allocator, &context->scImageViews[i]));
    }
}