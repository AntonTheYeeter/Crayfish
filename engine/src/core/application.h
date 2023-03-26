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

typedef struct Application
{
    b8 running;
    PlatformWindow window;
    Renderer renderer;
} Application;

CFAPI b8 appStartup(ApplicationConfig* config);
CFAPI void appRun();
CFAPI void appShutdown();

#endif