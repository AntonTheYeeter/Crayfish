#ifndef VULKAN_BACKEND_H
#define VULKAN_BACKEND_H

#include "def.h"
#include "platform/platform.h"

b8 vulkan_backendStartup(PlatformWindow* win);
void vulkan_backendShutdown();
void vulkan_backendDrawFrame();
void vulkan_backendOnResize(u32 newWidth, u32 newHeight);

#endif