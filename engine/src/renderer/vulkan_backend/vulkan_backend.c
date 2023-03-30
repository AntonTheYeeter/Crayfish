#include "vulkan_backend.h"

#include "vulkan_def.h"
#include "platform/platform.h"
#include "core/cf_memory.h"

static VulkanContext context;

#ifdef _DEBUG

VKAPI_ATTR VkBool32 VKAPI_CALL debugMessageCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageTypes, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) 
{
    switch(messageSeverity)
    {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            CF_ERROR("%s", pCallbackData->pMessage);
            break;
        
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            CF_WARN("%s", pCallbackData->pMessage);
            break;
        
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            CF_INFO("%s", pCallbackData->pMessage);
            break;
        
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            CF_TRACE("%s", pCallbackData->pMessage);
            break;

        default:
            break;
    }

    return VK_FALSE;
}

#endif

void createInstance()
{
    VkApplicationInfo appInfo = {VK_STRUCTURE_TYPE_APPLICATION_INFO};
    appInfo.apiVersion = VK_API_VERSION_1_3;

    VkInstanceCreateInfo instanceInfo = {VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
    instanceInfo.pApplicationInfo = &appInfo;

    #ifdef _DEBUG
    const char* instanceLayers[] =
    {
        "VK_LAYER_KHRONOS_validation"
    };
    instanceInfo.enabledLayerCount = ARRAY_SIZE(instanceLayers);
    instanceInfo.ppEnabledLayerNames = instanceLayers;
    #else
    instanceInfo.enabledLayerCount = 0;
    instanceInfo.ppEnabledLayerNames = PNULL;
    #endif

    const char* platformSurfaceExtension = getPlatformSurfaceExtenion();

    const char* instanceExtensions[] =
    {
        platformSurfaceExtension,
        VK_KHR_SURFACE_EXTENSION_NAME,
        #ifdef _DEBUG
        VK_EXT_DEBUG_UTILS_EXTENSION_NAME
        #endif
    };

    instanceInfo.enabledExtensionCount = ARRAY_SIZE(instanceExtensions);
    instanceInfo.ppEnabledExtensionNames = instanceExtensions;

    VK_CHECK(vkCreateInstance(&instanceInfo, context.allocator, &context.instance));
}

#ifdef _DEBUG
void createDebugMessenger()
{
    VkDebugUtilsMessengerCreateInfoEXT dmInfo = {VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT};
    dmInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
    VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
    VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
    VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT;

    dmInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
    VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

    dmInfo.pfnUserCallback = debugMessageCallback;

    PFN_vkCreateDebugUtilsMessengerEXT createFunc = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(context.instance, "vkCreateDebugUtilsMessengerEXT");
    createFunc(context.instance, &dmInfo, context.allocator, &context.debugMessenger);
}
#endif

void selectGPU()
{
    u32 gpuCount = 0;
    VK_CHECK(vkEnumeratePhysicalDevices(context.instance, &gpuCount, PNULL));
    VkPhysicalDevice gpus[gpuCount];
    VK_CHECK(vkEnumeratePhysicalDevices(context.instance, &gpuCount, gpus));

    b8 graphicsQueueFound = FALSE;
    b8 presentQueueFound = FALSE;
    b8 transferQueueFound = FALSE;

    for(u32 i = 0; i < gpuCount; i++)
    {
        VkPhysicalDeviceProperties props;
        vkGetPhysicalDeviceProperties(gpus[i], &props);

        u32 queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(gpus[i], &queueFamilyCount, PNULL);
        VkQueueFamilyProperties queueFamilies[queueFamilyCount];
        vkGetPhysicalDeviceQueueFamilyProperties(gpus[i], &queueFamilyCount, queueFamilies);

        if(props.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        {
            CF_WARN("This device doesn't have a discrete GPU which may lead to bugs!");
        }

        for(u32 j = 0; j < queueFamilyCount; j++)
        {
            VkBool32 presentSupport = VK_FALSE;
            VK_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(gpus[i], j, context.surface, &presentSupport));

            if(queueFamilies[j].queueFlags & VK_QUEUE_GRAPHICS_BIT && !graphicsQueueFound)
            {
                context.graphicsQueueIndex = j;
                graphicsQueueFound = TRUE;
            }
            else if(presentSupport && !presentQueueFound)
            {
                context.presentQueueIndex = j;
                presentQueueFound = TRUE;
            }
            else if(queueFamilies[j].queueFlags & VK_QUEUE_TRANSFER_BIT && !transferQueueFound)
            {
                context.transferQueueIndex = j;
                transferQueueFound = TRUE;
            }
        }

        if(graphicsQueueFound && presentQueueFound && transferQueueFound)
        {
            CF_DEBUG("Selected GPU %s", props.deviceName);
            CF_DEBUG("Graphics Queue Index: %i Present Queue Index: %i Transfer Queue Index: %i", context.graphicsQueueIndex, context.presentQueueIndex, context.transferQueueIndex);
            context.gpu = gpus[i];
            break;
        }
    }

    if(!graphicsQueueFound || !presentQueueFound || !transferQueueFound)
    {
        CF_FATAL("No suitable GPU found!");
    }
}

void createDevice()
{
    f32 queuePriority = 1.0f;

    VkDeviceQueueCreateInfo graphicsQueueInfo = {VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO};
    graphicsQueueInfo.queueFamilyIndex = context.graphicsQueueIndex;
    graphicsQueueInfo.queueCount = 1;
    graphicsQueueInfo.pQueuePriorities = &queuePriority;

    VkDeviceQueueCreateInfo presentQueueInfo = {VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO};
    presentQueueInfo.queueFamilyIndex = context.presentQueueIndex;
    presentQueueInfo.queueCount = 1;
    presentQueueInfo.pQueuePriorities = &queuePriority;

    VkDeviceQueueCreateInfo transferQueueInfo = {VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO};
    transferQueueInfo.queueFamilyIndex = context.transferQueueIndex;
    transferQueueInfo.queueCount = 1;
    transferQueueInfo.pQueuePriorities = &queuePriority;

    VkDeviceQueueCreateInfo queueInfos[] = {graphicsQueueInfo, presentQueueInfo, transferQueueInfo};

    VkDeviceCreateInfo deviceInfo = {VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO};
    deviceInfo.queueCreateInfoCount = ARRAY_SIZE(queueInfos);
    deviceInfo.pQueueCreateInfos = queueInfos;

    const char* deviceExtensions[] =
    {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    deviceInfo.enabledExtensionCount = ARRAY_SIZE(deviceExtensions);
    deviceInfo.ppEnabledExtensionNames = deviceExtensions;

    VkPhysicalDeviceFeatures features;
    vkGetPhysicalDeviceFeatures(context.gpu, &features);

    deviceInfo.pEnabledFeatures = &features;

    VK_CHECK(vkCreateDevice(context.gpu, &deviceInfo, context.allocator, &context.device));

    vkGetDeviceQueue(context.device, context.graphicsQueueIndex, 0, &context.graphicsQueue);
    vkGetDeviceQueue(context.device, context.presentQueueIndex, 0, &context.presentQueue);
    vkGetDeviceQueue(context.device, context.transferQueueIndex, 0, &context.transferQueue);
}

void createSwapchain()
{
    VkSurfaceCapabilitiesKHR surfaceCaps;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(context.gpu, context.surface, &surfaceCaps);

    VkSwapchainCreateInfoKHR scInfo = {VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR};
    scInfo.surface = context.surface;
    scInfo.minImageCount = surfaceCaps.minImageCount + 1 < surfaceCaps.maxImageCount ? surfaceCaps.minImageCount + 1 : surfaceCaps.minImageCount;

    {
        u32 formatCount = 0;
        vkGetPhysicalDeviceSurfaceFormatsKHR(context.gpu, context.surface, &formatCount, PNULL);
        VkSurfaceFormatKHR availableFormats[formatCount];
        vkGetPhysicalDeviceSurfaceFormatsKHR(context.gpu, context.surface, &formatCount, availableFormats);

        b8 foundFormat = FALSE;

        for(u32 i = 0; i < formatCount; i++)
        {
            if(availableFormats[i].format == VK_FORMAT_B8G8R8A8_UNORM && !foundFormat)
            {
                context.scFormat = availableFormats[i];
                foundFormat = TRUE;
                break;
            }
        }

        if(!foundFormat)
        {
            context.scFormat = availableFormats[0];
        }
    }

    scInfo.imageFormat = context.scFormat.format;
    scInfo.imageColorSpace = context.scFormat.colorSpace;
    scInfo.imageExtent = surfaceCaps.currentExtent;
    scInfo.imageArrayLayers = 1;
    scInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    scInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    scInfo.queueFamilyIndexCount = 2;

    u32 queueFamilies[] = {context.graphicsQueueIndex, context.presentQueueIndex};

    scInfo.pQueueFamilyIndices = queueFamilies;
    scInfo.preTransform = surfaceCaps.currentTransform;
    scInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

    VkPresentModeKHR presentMode;
    {
        u32 presentModeCount = 0;
        vkGetPhysicalDeviceSurfacePresentModesKHR(context.gpu, context.surface, &presentModeCount, PNULL);
        VkPresentModeKHR availablePresentModes[presentModeCount];
        vkGetPhysicalDeviceSurfacePresentModesKHR(context.gpu, context.surface, &presentModeCount, availablePresentModes);

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
    scInfo.oldSwapchain = VK_NULL_HANDLE;

    VK_CHECK(vkCreateSwapchainKHR(context.device, &scInfo, context.allocator, &context.swapchain));

    VK_CHECK(vkGetSwapchainImagesKHR(context.device, context.swapchain, &context.scImageCount, PNULL));
    context.scImages = cfAllocate(context.scImageCount * sizeof(VkImage));
    VK_CHECK(vkGetSwapchainImagesKHR(context.device, context.swapchain, &context.scImageCount, context.scImages));
}

void createImageViews()
{
    VkImageViewCreateInfo imageViewInfo = {VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
    imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    imageViewInfo.format = context.scFormat.format;
    imageViewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imageViewInfo.subresourceRange.levelCount = 1;
    imageViewInfo.subresourceRange.layerCount = 1;

    context.scImageViews = cfAllocate(context.scImageCount * sizeof(VkImageView));

    for(u32 i = 0; i < context.scImageCount; i++)
    {
        imageViewInfo.image = context.scImages[i];
        VK_CHECK(vkCreateImageView(context.device, &imageViewInfo, context.allocator, &context.scImageViews[i]));
    }
}

void createRenderPass()
{
    VkAttachmentDescription attachment = {};
    attachment.format = context.scFormat.format;
    attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachment = {};
    colorAttachment.attachment = 0;
    colorAttachment.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachment;

    VkRenderPassCreateInfo rpInfo = {VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO};
    rpInfo.attachmentCount = 1;
    rpInfo.pAttachments = &attachment;
    rpInfo.subpassCount = 1;
    rpInfo.pSubpasses = &subpass;
    rpInfo.dependencyCount = 0; // TODO:
    rpInfo.pDependencies = PNULL;// TODO:

    VK_CHECK(vkCreateRenderPass(context.device, &rpInfo, context.allocator, &context.renderPass));
}

void createFramebuffers()
{
    VkFramebufferCreateInfo fbInfo = {VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO};
    fbInfo.renderPass = context.renderPass;
    fbInfo.width = context.windowExtent.width;
    fbInfo.height = context.windowExtent.height;
    fbInfo.layers = 1;

    context.scFramebuffers = cfAllocate(context.scImageCount * sizeof(VkFramebuffer));

    for(u32 i = 0; i < context.scImageCount; i++)
    {
        fbInfo.attachmentCount = 1;
        fbInfo.pAttachments = &context.scImageViews[i];

        VK_CHECK(vkCreateFramebuffer(context.device, &fbInfo, context.allocator, &context.scFramebuffers[i]));
    }
}

void createCommandPool()
{
    VkCommandPoolCreateInfo commandPoolInfo = {VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO};
    commandPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    commandPoolInfo.queueFamilyIndex = context.graphicsQueueIndex;

    VK_CHECK(vkCreateCommandPool(context.device, &commandPoolInfo, context.allocator, &context.commandPool));
}

void allocateCommandBuffer()
{
    VkCommandBufferAllocateInfo cmdAllocInfo = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
    cmdAllocInfo.commandPool = context.commandPool;
    cmdAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    cmdAllocInfo.commandBufferCount = 1;

    VK_CHECK(vkAllocateCommandBuffers(context.device, &cmdAllocInfo, &context.commandBuffer));
}

void createSyncObjects()
{
    VkSemaphoreCreateInfo semaInfo = {VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};

    VK_CHECK(vkCreateSemaphore(context.device, &semaInfo, context.allocator, &context.imageAvailableSemaphore));
    VK_CHECK(vkCreateSemaphore(context.device, &semaInfo, context.allocator, &context.renderFinishedSemaphore));

    VkFenceCreateInfo fenceInfo = {VK_STRUCTURE_TYPE_FENCE_CREATE_INFO};
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    VK_CHECK(vkCreateFence(context.device, &fenceInfo, context.allocator, &context.inFlightFence));
}

void recreateSwapchain(u32 w, u32 h)
{
    vkDeviceWaitIdle(context.device);

    context.windowExtent.width = w;
    context.windowExtent.height = h;

    for(u32 i = 0; i < context.scImageCount; i++)
    {
        vkDestroyFramebuffer(context.device, context.scFramebuffers[i], context.allocator);
    }

    for(u32 i = 0; i < context.scImageCount; i++)
    {
        vkDestroyImageView(context.device, context.scImageViews[i], context.allocator);
    }

    vkDestroySwapchainKHR(context.device, context.swapchain, context.allocator);

    createSwapchain();
    createImageViews();
    createFramebuffers();
}

b8 vulkan_rendererBackendStartup(PlatformWindow* win, u32 windowWidth, u32 windowHeight)
{
    context.windowExtent.width = windowWidth;
    context.windowExtent.height = windowHeight;

    createInstance();
    #ifdef _DEBUG
    createDebugMessenger();
    #endif
    context.surface = createSurface(win, context.instance, context.allocator);
    selectGPU();
    createDevice();
    createSwapchain();
    createImageViews();
    createRenderPass();
    createFramebuffers();
    createCommandPool();
    allocateCommandBuffer();
    createSyncObjects();

    return TRUE;
}

void vulkan_rendererBackendShutdown()
{
    vkDeviceWaitIdle(context.device);

    vkDestroyFence(context.device, context.inFlightFence, context.allocator);
    vkDestroySemaphore(context.device, context.renderFinishedSemaphore, context.allocator);
    vkDestroySemaphore(context.device, context.imageAvailableSemaphore, context.allocator);

    vkFreeCommandBuffers(context.device, context.commandPool, 1, &context.commandBuffer);
    vkDestroyCommandPool(context.device, context.commandPool, context.allocator);

    for(u32 i = 0; i < context.scImageCount; i++)
    {
        vkDestroyFramebuffer(context.device, context.scFramebuffers[i], context.allocator);
    }

    vkDestroyRenderPass(context.device, context.renderPass, context.allocator);

    for(u32 i = 0; i < context.scImageCount; i++)
    {
        vkDestroyImageView(context.device, context.scImageViews[i], context.allocator);
    }
    cfFree(context.scImageViews);
    cfFree(context.scImages);

    vkDestroySwapchainKHR(context.device, context.swapchain, context.allocator);
    vkDestroyDevice(context.device, context.allocator);
    vkDestroySurfaceKHR(context.instance, context.surface, context.allocator);

    if(context.debugMessenger != VK_NULL_HANDLE)
    {
        PFN_vkDestroyDebugUtilsMessengerEXT destroyFunc = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(context.instance, "vkDestroyDebugUtilsMessengerEXT");

        destroyFunc(context.instance, context.debugMessenger, context.allocator);
    }

    vkDestroyInstance(context.instance, context.allocator);
}

void vulkan_rendererBackendDrawFrame(f32 delta)
{
    vkWaitForFences(context.device, 1, &context.inFlightFence, VK_TRUE, U64_MAX);
    vkResetFences(context.device, 1, &context.inFlightFence);

    u32 imageIndex = 0;
    VkResult res = vkAcquireNextImageKHR(context.device, context.swapchain, U64_MAX, context.imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

    if(res == VK_ERROR_OUT_OF_DATE_KHR)
    {
        return;
    }

    VK_CHECK(vkResetCommandBuffer(context.commandBuffer, 0));

    VkCommandBufferBeginInfo cmdBeginInfo = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};

    VK_CHECK(vkBeginCommandBuffer(context.commandBuffer, &cmdBeginInfo));

    VkRenderPassBeginInfo rpBeginInfo = {VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO};
    rpBeginInfo.renderPass = context.renderPass;
    rpBeginInfo.framebuffer = context.scFramebuffers[imageIndex];

    VkRect2D renderArea = {};
    renderArea.extent = context.windowExtent;

    rpBeginInfo.renderArea = renderArea;

    VkClearColorValue clearColor = {1.0f, 0.0f, 0.0f, 1.0f};

    VkClearValue clearValue = {};
    clearValue.color = clearColor;

    rpBeginInfo.clearValueCount = 1;
    rpBeginInfo.pClearValues = &clearValue;

    vkCmdBeginRenderPass(context.commandBuffer, &rpBeginInfo, 0);

    {

    }

    vkCmdEndRenderPass(context.commandBuffer);

    VK_CHECK(vkEndCommandBuffer(context.commandBuffer));

    VkSubmitInfo submitInfo = {VK_STRUCTURE_TYPE_SUBMIT_INFO};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &context.imageAvailableSemaphore;

    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &context.commandBuffer;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &context.renderFinishedSemaphore;

    VK_CHECK(vkQueueSubmit(context.graphicsQueue, 1, &submitInfo, context.inFlightFence));

    VkPresentInfoKHR presentInfo = {VK_STRUCTURE_TYPE_PRESENT_INFO_KHR};
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &context.renderFinishedSemaphore;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &context.swapchain;
    presentInfo.pImageIndices = &imageIndex;

    res = vkQueuePresentKHR(context.presentQueue, &presentInfo);

    if(res == VK_ERROR_OUT_OF_DATE_KHR || res == VK_SUBOPTIMAL_KHR)
    {
        return;
    }
}

void vulkan_rendererBackendOnResize(u32 newWidth, u32 newHeight)
{
    recreateSwapchain(newWidth, newHeight);
}