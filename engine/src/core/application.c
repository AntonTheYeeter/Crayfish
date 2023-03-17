#include "application.h"
#include "logger.h"
#include "core/event.h"

#include <stdlib.h>

Application* createApp(ApplicationConfig* config)
{
    Application* app = malloc(sizeof(Application));

    platformCreateWindow(
    &app->platform.window,
    config->windowX,
    config->windowY,
    config->windowWidth,
    config->windowHeight,
    config->appName
    );
    
    // TODO: Make this configurable
    rendererStartup(&app->renderer, RENDERER_BACKEND_VULKAN, &app->platform.window);

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
        rendererDrawFrame(&app->renderer);
        platformWindowUpdate(&app->platform.window);

        Event e = pollEvents();

        if(e.type & EVENT_TYPE_WINDOW_RESIZED)
        {
            u32 w, h;
            getWindowSize(&w, &h);

            rendererOnResize(&app->renderer, w, h);

            CF_INFO("Window Resized | Width: %u Height: %u", w, h);
        }

        resetEvent(&e);
    }
    while(!app->platform.window.hasClosed);
}
    

void destroyApp(Application* app)
{
    rendererShutdown(&app->renderer);
    platformDestroyWindow(&app->platform.window);

    free(app);
    
    CF_INFO("Application closed.");
}