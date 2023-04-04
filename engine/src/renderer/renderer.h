#ifndef RENDERER_H
#define RENDERER_H

#include "def.h"
#include "platform/platform.h"
#include "core/cf_math.h"

typedef struct Vertex
{
    Vec3f position;
    Vec4f color;
    Vec2f texCoord;
    Vec3f normal;
} Vertex;

typedef struct Mesh
{
    u32 numVertices;
    Vertex* vertices;

    b8 isVisible;
} Mesh;

typedef b8 (*PFN_rendererBackendStartup)(PlatformWindow* win, u32 windowWidth, u32 windowHeight);
typedef void (*PFN_rendererBackendShutdown)();
typedef void (*PFN_rendererBackendDrawFrame)(f32 delta);
typedef void (*PFN_rendererBackendOnResize)(u32 newWidth, u32 newHeight);
typedef void (*PFN_rendererBackendAddMeshData)(u32 meshCount, Mesh* meshes);

typedef struct Renderer
{
    PFN_rendererBackendStartup backendStartup;
    PFN_rendererBackendShutdown backendShutdown;
    PFN_rendererBackendDrawFrame backendDrawFrame;
    PFN_rendererBackendOnResize backendOnResize;
    PFN_rendererBackendAddMeshData backendAddMeshData;
} Renderer;

typedef enum BackendType
{
    RENDERER_BACKEND_TYPE_VULKAN,
    RENDERER_BACKEND_TYPE_OPENGL
} BackendType;

b8 rendererStartup(PlatformWindow* win, Renderer* r, BackendType backend, u32 windowWidth, u32 windowHeight);
void rendererShutdown(Renderer* r);
void rendererDrawFrame(Renderer* r, f32 delta);
void rendererOnResize(Renderer* r, u32 newWidth, u32 newHeight);
void rendererAddMeshData(Renderer* r, u32 meshCount, Mesh* meshes);

#endif