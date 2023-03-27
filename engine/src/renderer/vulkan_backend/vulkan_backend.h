#ifndef VULKAN_BACKEND_H
#define VULKAN_BACKEND_H

#include "renderer/renderer.h"

b8 vulkan_rendererBackendStartup(PlatformWindow* win, u32 windowWidth, u32 windowHeight);
void vulkan_rendererBackendShutdown();
void vulkan_rendererBackendDrawFrame(f32 delta);
void vulkan_rendererBackendOnResize(u32 newWidth, u32 newHeight);

#endif