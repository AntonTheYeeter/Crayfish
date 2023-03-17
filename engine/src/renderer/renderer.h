#ifndef RENDERER_H
#define RENDERER_H

#include "def.h"
#include "platform/platform.h"

typedef b8 (*PFN_backendStartup)(PlatformWindow* win);
typedef void (*PFN_backendShutdown)();
typedef void (*PFN_backendDrawFrame)();
typedef void (*PFN_backendOnResize)(u32 newWidth, u32 newHeight);

typedef struct Renderer
{
    PFN_backendStartup backendStartup;
    PFN_backendShutdown backendShutdown;
    PFN_backendDrawFrame backendDrawFrame;
    PFN_backendOnResize backendOnResize;
} Renderer;

typedef enum RendererBackendType
{
    RENDERER_BACKEND_VULKAN
} RendererBackendType;

b8 rendererStartup(Renderer* r, RendererBackendType backend, PlatformWindow* win);
void rendererShutdown(Renderer* r);
void rendererDrawFrame(Renderer* r);
void rendererOnResize(Renderer* r, u32 newWidth, u32 newHeight);

#endif