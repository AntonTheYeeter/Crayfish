#ifndef RENDERER_H
#define RENDERER_H

#include "def.h"

typedef b8 (*PFN_rendererBackendStartup)(u32 windowWidth, u32 windowHeight);
typedef void (*PFN_rendererBackendShutdown)();
typedef void (*PFN_rendererBackendDrawFrame)(f32 delta);
typedef void (*PFN_rendererBackendOnResize)(u32 newWidth, u32 newHeight);

typedef struct Renderer
{
    PFN_rendererBackendStartup backendStartup;
    PFN_rendererBackendShutdown backendShutdown;
    PFN_rendererBackendDrawFrame backendDrawFrame;
    PFN_rendererBackendOnResize backendOnResize;
} Renderer;

typedef enum BackendType
{
    RENDERER_BACKEND_TYPE_VULKAN,
    RENDERER_BACKEND_TYPE_OPENGL,
    RENDERER_BACKEND_TYPE_DIRECTX
} BackendType;

b8 rendererStartup(Renderer* r, BackendType backend, u32 windowWidth, u32 windowHeight);
void rendererShutdown(Renderer* r);
void rendererDrawFrame(Renderer* r, f32 delta);
void rendererOnResize(Renderer* r, u32 newWidth, u32 newHeight);

#endif