#include <entry.h>

ApplicationConfig configApp()
{
    ApplicationConfig config;
    config.windowPosX = 300;
    config.windowPosY = 300;
    config.windowWidth = 1280;
    config.windowHeight = 720;
    config.applicationName = "Crayfish Testapp";

    return config;
}