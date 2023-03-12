#ifndef APPLICATION_H
#define APPLICATION_H

#include "def.h"
#include "platform/platform.h"

// TODO: multiple window support

typedef struct ApplicationConfig
{
    u32 windowX;
    u32 windowY;
    u32 windowWidth;
    u32 windowHeight;
    const char* appName;
} ApplicationConfig;

typedef struct Application
{
    PlatformState platform;
} Application;

CFAPI Application* createApp(ApplicationConfig* config);
CFAPI void runApp(Application* app);
CFAPI void destroyApp(Application* app);

#endif