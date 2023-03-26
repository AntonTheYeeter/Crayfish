#include "renderer.h"
#include "core/logger.h"
#include "vulkan_backend/vulkan_backend.h"

b8 rendererStartup(Renderer* r, BackendType backend, u32 windowWidth, u32 windowHeight)
{
    if(backend == RENDERER_BACKEND_TYPE_VULKAN)
    {
        r->backendStartup = vulkan_rendererBackendStartup;
        r->backendShutdown = vulkan_rendererBackendShutdown;
        r->backendDrawFrame = vulkan_rendererBackendDrawFrame;
        r->backendOnResize = vulkan_rendererBackendOnResize;
    }
    else
    {
        CF_FATAL("Crayfish only supports Vulkan!");
        return FALSE;
    }

    r->backendStartup(windowWidth, windowHeight);

    return TRUE;
}

void rendererShutdown(Renderer* r)
{
    r->backendShutdown();
}

void rendererDrawFrame(Renderer* r, f32 delta)
{
    r->backendDrawFrame(delta);
}

void rendererOnResize(Renderer* r, u32 newWidth, u32 newHeight)
{
    r->backendOnResize(newWidth, newHeight);
}