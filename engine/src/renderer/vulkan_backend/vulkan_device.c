#include "vulkan_device.h"

#include "platform/platform.h"

void createInstance(VulkanContext* context)
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

    VK_CHECK(vkCreateInstance(&instanceInfo, context->allocator, &context->instance));
}

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

void createDebugMessenger(VulkanContext* context)
{
    VkDebugUtilsMessengerCreateInfoEXT dmInfo = {VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT};
    dmInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
    VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
    VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
    VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT;

    dmInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | 
    VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
    VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

    dmInfo.pfnUserCallback = debugMessageCallback;

    PFN_vkCreateDebugUtilsMessengerEXT createFunc = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(context->instance, "vkCreateDebugUtilsMessengerEXT");
    createFunc(context->instance, &dmInfo, context->allocator, &context->debugMessenger);
}
#endif

void selectGPU(VulkanContext* context)
{
    u32 gpuCount = 0;
    VK_CHECK(vkEnumeratePhysicalDevices(context->instance, &gpuCount, PNULL));
    VkPhysicalDevice gpus[gpuCount];
    VK_CHECK(vkEnumeratePhysicalDevices(context->instance, &gpuCount, gpus));

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
            VK_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(gpus[i], j, context->surface, &presentSupport));

            if(queueFamilies[j].queueFlags & VK_QUEUE_GRAPHICS_BIT && !graphicsQueueFound)
            {
                context->graphicsQueueIndex = j;
                graphicsQueueFound = TRUE;
            }
            else if(presentSupport && !presentQueueFound)
            {
                context->presentQueueIndex = j;
                presentQueueFound = TRUE;
            }
            else if(queueFamilies[j].queueFlags & VK_QUEUE_TRANSFER_BIT && !transferQueueFound)
            {
                context->transferQueueIndex = j;
                transferQueueFound = TRUE;
            }
        }

        if(graphicsQueueFound && presentQueueFound && transferQueueFound)
        {
            CF_DEBUG("Selected GPU %s", props.deviceName);
            CF_DEBUG("Graphics Queue Index: %i Present Queue Index: %i Transfer Queue Index: %i", context->graphicsQueueIndex, context->presentQueueIndex, context->transferQueueIndex);
            context->gpu = gpus[i];
            break;
        }
    }

    if(!graphicsQueueFound || !presentQueueFound || !transferQueueFound)
    {
        CF_FATAL("No suitable GPU found!");
    }
}

void createDevice(VulkanContext* context)
{
    f32 queuePriority = 1.0f;

    VkDeviceQueueCreateInfo graphicsQueueInfo = {VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO};
    graphicsQueueInfo.queueFamilyIndex = context->graphicsQueueIndex;
    graphicsQueueInfo.queueCount = 1;
    graphicsQueueInfo.pQueuePriorities = &queuePriority;

    VkDeviceQueueCreateInfo presentQueueInfo = {VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO};
    presentQueueInfo.queueFamilyIndex = context->presentQueueIndex;
    presentQueueInfo.queueCount = 1;
    presentQueueInfo.pQueuePriorities = &queuePriority;

    VkDeviceQueueCreateInfo transferQueueInfo = {VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO};
    transferQueueInfo.queueFamilyIndex = context->transferQueueIndex;
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
    vkGetPhysicalDeviceFeatures(context->gpu, &features);

    deviceInfo.pEnabledFeatures = &features;

    VK_CHECK(vkCreateDevice(context->gpu, &deviceInfo, context->allocator, &context->device));

    vkGetDeviceQueue(context->device, context->graphicsQueueIndex, 0, &context->graphicsQueue);
    vkGetDeviceQueue(context->device, context->presentQueueIndex, 0, &context->presentQueue);
    vkGetDeviceQueue(context->device, context->transferQueueIndex, 0, &context->transferQueue);
}