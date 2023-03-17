#include "vulkan_device.h"
#include "vulkan_platform.h"

static VKAPI_ATTR VkBool32 VKAPI_CALL debugMessageCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
VkDebugUtilsMessageTypeFlagsEXT messageTypes,
const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
void *pUserData)
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

b8 createDevice(VulkanContext* context, PlatformWindow* win)
{
    // Instance
    {
        VkApplicationInfo appInfo = {VK_STRUCTURE_TYPE_APPLICATION_INFO};
        appInfo.apiVersion = VK_API_VERSION_1_3;

        const char* instanceExtensions[] =
        {
            VK_KHR_SURFACE_EXTENSION_NAME,
            PLATFORM_SURFACE_EXTENSION,
            VK_EXT_DEBUG_UTILS_EXTENSION_NAME
        };

        const char* instanceLayers[] =
        {
            "VK_LAYER_KHRONOS_validation"
        };

        VkInstanceCreateInfo instanceInfo = {VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
        instanceInfo.pApplicationInfo = &appInfo;
        instanceInfo.enabledLayerCount = ARRAY_SIZE(instanceLayers);
        instanceInfo.ppEnabledLayerNames = instanceLayers;
        instanceInfo.enabledExtensionCount = ARRAY_SIZE(instanceExtensions);
        instanceInfo.ppEnabledExtensionNames = instanceExtensions;

        VK_CHECK(vkCreateInstance(&instanceInfo, context->allocator, &context->instance));
        CF_DEBUG("Vulkan Instance created successfully.");
    }

    // Debugger
    {
        VkDebugUtilsMessengerCreateInfoEXT debuggerInfo = {VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT};
        debuggerInfo.messageSeverity = 
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT;

        debuggerInfo.messageType =
        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

        debuggerInfo.pfnUserCallback = debugMessageCallback;

        PFN_vkCreateDebugUtilsMessengerEXT createFunc = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(context->instance, "vkCreateDebugUtilsMessengerEXT");
        VK_CHECK(createFunc(context->instance, &debuggerInfo, context->allocator, &context->debugger));
    }

    // Surface
    {
        createSurface(win, context);
    }

    // Physical device
    {
        u32 gpuCount = 0;
        VK_CHECK(vkEnumeratePhysicalDevices(context->instance, &gpuCount, NULL));
        VkPhysicalDevice gpus[gpuCount];
        VK_CHECK(vkEnumeratePhysicalDevices(context->instance, &gpuCount, gpus));

        for(u32 i = 0; i < gpuCount; i++)
        {
            VkPhysicalDeviceProperties props;
            vkGetPhysicalDeviceProperties(gpus[i], &props);

            if(props.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            {
                CF_WARN("This device currently uses an integrated GPU. Integrated GPUs may not perform well on this engine.");
            }

            u32 queueFamilyCount = 0;
            vkGetPhysicalDeviceQueueFamilyProperties(gpus[i], &queueFamilyCount, NULL);
            VkQueueFamilyProperties queueFamilyProps[queueFamilyCount];
            vkGetPhysicalDeviceQueueFamilyProperties(gpus[i], &queueFamilyCount, queueFamilyProps);

            b8 foundGraphicsQueueIndex = FALSE;
            b8 foundPresentQueueIndex = FALSE;
            b8 foundTransferQueueIndex = FALSE;

            for(u32 j = 0; j < queueFamilyCount; j++)
            {
                VkBool32 presentSupport = VK_FALSE;
                vkGetPhysicalDeviceSurfaceSupportKHR(gpus[i], j, context->surface, &presentSupport);

                if((queueFamilyProps->queueFlags & VK_QUEUE_GRAPHICS_BIT) && (foundGraphicsQueueIndex == FALSE))
                {
                    context->graphicsQueue.familyIndex = j;
                    foundGraphicsQueueIndex = TRUE;
                }
                else if(presentSupport && (foundPresentQueueIndex == FALSE))
                {
                    context->presentQueue.familyIndex = j;
                    foundPresentQueueIndex = TRUE;
                }
                else if((queueFamilyProps->queueFlags & VK_QUEUE_TRANSFER_BIT) && (foundTransferQueueIndex == FALSE))
                {
                    context->transferQueue.familyIndex = j;
                    foundTransferQueueIndex = TRUE;
                }
            }

            if((foundGraphicsQueueIndex == FALSE) ||
            (foundPresentQueueIndex == FALSE) ||
            (foundTransferQueueIndex == FALSE))
            {
                CF_FATAL("No suitable GPU found!");
                return FALSE;
            }

            CF_DEBUG("Selecting GPU: %s", props.deviceName);
            context->gpu = gpus[i];
            CF_DEBUG("Graphics Queue Family Index: %u", context->graphicsQueue.familyIndex);
            CF_DEBUG("Present Queue Family Index: %u", context->presentQueue.familyIndex);
            CF_DEBUG("Transfer Queue Family Index: %u", context->transferQueue.familyIndex);
        }
    }

    // Logical device
    {
        f32 queuePriotity = 1.0f;

        VkDeviceQueueCreateInfo graphicsQueueInfo = {VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO};
        graphicsQueueInfo.queueFamilyIndex = context->graphicsQueue.familyIndex;
        graphicsQueueInfo.queueCount = 1;
        graphicsQueueInfo.pQueuePriorities = &queuePriotity;

        VkDeviceQueueCreateInfo presentQueueInfo = {VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO};
        presentQueueInfo.queueFamilyIndex = context->presentQueue.familyIndex;
        presentQueueInfo.queueCount = 1;
        presentQueueInfo.pQueuePriorities = &queuePriotity;

        VkDeviceQueueCreateInfo transferQueueInfo = {VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO};
        transferQueueInfo.queueFamilyIndex = context->transferQueue.familyIndex;
        transferQueueInfo.queueCount = 1;
        transferQueueInfo.pQueuePriorities = &queuePriotity;

        VkDeviceQueueCreateInfo queueInfos[] = {graphicsQueueInfo, presentQueueInfo, transferQueueInfo};

        VkDeviceCreateInfo deviceInfo = {VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO};
        deviceInfo.queueCreateInfoCount = 3;
        deviceInfo.pQueueCreateInfos = queueInfos;
        deviceInfo.enabledLayerCount = 0;
        deviceInfo.ppEnabledLayerNames = NULL;

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

        vkGetDeviceQueue(context->device, context->graphicsQueue.familyIndex, 0, &context->graphicsQueue.handle);
        vkGetDeviceQueue(context->device, context->presentQueue.familyIndex, 0, &context->presentQueue.handle);
        vkGetDeviceQueue(context->device, context->transferQueue.familyIndex, 0, &context->transferQueue.handle);
    }

    return TRUE;
}

void destroyDevice(VulkanContext* context)
{

    vkDestroyDevice(context->device, context->allocator);
    vkDestroySurfaceKHR(context->instance, context->surface, context->allocator);
    {
        PFN_vkDestroyDebugUtilsMessengerEXT destroyFunc = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(context->instance, "vkDestroyDebugUtilsMessengerEXT");
        destroyFunc(context->instance, context->debugger, context->allocator);
    }

    vkDestroyInstance(context->instance, context->allocator);
}