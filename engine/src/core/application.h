#ifndef APPLICATION_H
#define APPLICATION_H

#include "def.h"
#include "platform/platform.h"

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
    PlatformWindow window;
    b8 running;
} Application;

CFAPI b8 appStartup(ApplicationConfig* config);
CFAPI void appRun();
CFAPI void appShutdown();

#endif