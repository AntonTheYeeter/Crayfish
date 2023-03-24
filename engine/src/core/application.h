#ifndef APPLICATION_H
#define APPLICATION_H

#include "def.h"

typedef struct ApplicationConfig
{
    u32 windowPosX;
    u32 windowPosY;
    u32 windowWidth;
    u32 windowheight;
    const char* applicationName;
} ApplicationConfig;

typedef struct Application
{

} Application;

CFAPI b8 appStartup(ApplicationConfig* config, Application* app);
CFAPI void appRun();
CFAPI void appShutdown();

#endif