#ifndef APPLICATION_H
#define APPLICATION_H

#include "def.h"
#include "platform/platform.h"
#include "renderer/renderer.h"

typedef struct ApplicationConfig
{
    u32 windowPosX;
    u32 windowPosY;
    u32 windowWidth;
    u32 windowHeight;
    const char* applicationName;
} ApplicationConfig;

#define MAX_MESHES 32000

typedef void (*PFN_gameStart)();
typedef void (*PFN_gameUpdate)(f32 delta);

#define MAX_MESHES 32000

typedef struct Application
{
    b8 running;
    PlatformWindow window;
    Renderer renderer;

    PFN_gameStart start;
    PFN_gameUpdate update;

    u32 meshCount;
    Mesh meshes[MAX_MESHES];
} Application;

CFAPI b8 appStartup(ApplicationConfig* config, PFN_gameStart start, PFN_gameUpdate update);
CFAPI void appRun();
CFAPI void appShutdown();
CFAPI void addMesh(Mesh mesh);

#endif