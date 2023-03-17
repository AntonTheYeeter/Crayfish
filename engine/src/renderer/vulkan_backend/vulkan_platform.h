#ifndef VULKAN_PLATFORM_H
#define VULKAN_PLATFORM_H

#include "platform/platform.h"
#include "platform/platform_check.h"
#include "vulkan_def.h"

#include <vulkan/vulkan.h>

#if defined(CF_PLATFORM_WINDOWS)
#define PLATFORM_SURFACE_EXTENSION "VK_KHR_win32_surface"
#elif defined(CF_PLATFORM_LINUX)
#define PLATFORM_SURFACE_EXTENSION "VK_KHR_xcb_surface"
#endif

void createSurface(PlatformWindow* win, VulkanContext* context);

#endif