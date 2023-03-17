#include "renderer.h"

#include "vulkan_backend/vulkan_backend.h"

b8 rendererStartup(Renderer* r, RendererBackendType backend, PlatformWindow* win)
{
    if(backend == RENDERER_BACKEND_VULKAN)
    {
        r->backendStartup = vulkan_backendStartup;
        r->backendShutdown = vulkan_backendShutdown;
        r->backendDrawFrame = vulkan_backendDrawFrame;
        r->backendOnResize = vulkan_backendOnResize;
    }

    r->backendStartup(win);

    return TRUE;
}

void rendererShutdown(Renderer* r)
{
    r->backendShutdown();
}

void rendererDrawFrame(Renderer* r)
{
    r->backendDrawFrame();
}

void rendererOnResize(Renderer* r, u32 newWidth, u32 newHeight)
{
    r->backendOnResize(newWidth, newHeight);
}