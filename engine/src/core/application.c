#include "application.h"
#include "logger.h"
#include "assertion.h"
#include "cf_memory.h"
#include "event.h"

static Application app;

void onWindowClose(EventCode code, void* data);
void onWindowResize(EventCode code, void* data);

b8 appStartup(ApplicationConfig* config)
{
    CF_ASSERT(loggerStartup);
    cfZeroMemory(&app.window, sizeof(PlatformWindow));
    CF_ASSERT(platformCreateWindow(&app.window, config->windowPosX, config->windowPosY, config->windowWidth, config->windowHeight, config->applicationName));
    CF_ASSERT(eventsStartup());
    CF_ASSERT(rendererStartup(&app.window, &app.renderer, RENDERER_BACKEND_TYPE_VULKAN/*TODO: Make this configurable*/, config->windowWidth, config->windowHeight));

    addEvent(EVENT_CODE_WINDOW_CLOSED, onWindowClose);
    addEvent(EVENT_CODE_WINDOW_RESIZED, onWindowResize);

    app.running = TRUE;

    return TRUE;
}

void appRun()
{
    f64 startTime = 0;
    f64 deltaTime = 0;

    while(app.running)
    {
        startTime = platformGetTime();

        rendererDrawFrame(&app.renderer, deltaTime);
        platformWindowUpdate(&app.window);

        deltaTime = platformGetTime() - startTime;
    }
}

void appShutdown()
{
    removeEvent(EVENT_CODE_WINDOW_CLOSED);

    rendererShutdown(&app.renderer);
    eventsShutdown();
    platformDestroyWindow(&app.window);
    loggerShutdown();
}

void onWindowClose(EventCode code, void* data)
{
    app.running = FALSE;
}

void onWindowResize(EventCode code, void* data)
{
    CF_INFO("Window resized. Width: %i Height: %i", ((u32*)data)[0], ((u32*)data)[1]);
}