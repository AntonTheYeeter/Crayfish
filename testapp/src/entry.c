#include <entry.h>

#include <stdlib.h>

ApplicationConfig configApp()
{
    ApplicationConfig config = {};
    config.windowX = 100;
    config.windowY = 100;
    config.windowWidth = 1280;
    config.windowHeight = 720;
    config.appName = "Crawfish Testapp";

    return config;
}