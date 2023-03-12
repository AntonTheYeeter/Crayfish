#include "application.h"
#include "logger.h"
#include "platform/platform.h"

#include <stdlib.h>

Application* createApp(ApplicationConfig* config)
{
    Application* app = malloc(sizeof(Application));

    platformCreateWindow(
    &app->platform.windows[0],
    config->windowX,
    config->windowY,
    config->windowWidth,
    config->windowHeight,
    config->appName
    );

    return app;
}

void runApp(Application* app)
{
    /*
    The game loop is a do-while loop because on windows there is a seperate function for polling window events.
    Because of this my implementation uses a global static boolean which is set to true if the window closed.
    PlatformWindow.hasClosed is then set to that boolean in platformWindowUpdate().
    This means you have to call platformWindowUpdate() at least once or the application will close immediatly.
    */
    do
    {
        platformWindowUpdate(&app->platform.windows[0]);
    }
    while(!(app->platform.windows[0].hasClosed));
}
    

void destroyApp(Application* app)
{
    platformDestroyWindow(&app->platform.windows[0]);

    free(app);
}