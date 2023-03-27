#include "vulkan_backend.h"

#include "vulkan_def.h"
#include "platform/platform.h"

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

b8 vulkan_rendererBackendStartup(PlatformWindow* win, u32 windowWidth, u32 windowHeight)
{
    // Instance
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

    //Debug Messenger
    #ifdef _DEBUG
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

        PFN_vkCreateDebugUtilsMessengerEXT createFunc = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(context.instance, "vkCreateDebugUtilsMessengerEXT");
        createFunc(context.instance, &dmInfo, context.allocator, &context.debugMessenger);
    }
    #endif

    // Surface
    {
        context.surface = createSurface(win, context.instance, context.allocator);
    }

    return TRUE;
}

void vulkan_rendererBackendShutdown()
{
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

}

void vulkan_rendererBackendOnResize(u32 newWidth, u32 newHeight)
{

}