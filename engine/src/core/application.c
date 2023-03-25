#include "application.h"
#include "logger.h"
#include "assertion.h"

b8 appStartup(ApplicationConfig* config, Application* app)
{
    CF_ASSERT(loggerStartup);
    CF_ASSERT(platformCreateWindow(&app->window, config->windowPosX, config->windowPosY, config->windowWidth, config->windowHeight, config->applicationName));

    return TRUE;
}

void appRun(Application* app)
{
    CF_FATAL("Test message %i", 1234);
    CF_ERROR("Test message %i", 1234);
    CF_WARN("Test message %i", 1234);
    CF_INFO("Test message %i", 1234);
    CF_DEBUG("Test message %i", 1234);
    CF_TRACE("Test message %i", 1234);

    while(TRUE);
}

void appShutdown(Application* app)
{
    platformDestroyWindow(&app->window);
    loggerShutdown();
}