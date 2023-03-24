#ifndef ENTRY_H
#define ENTRY_H

#include "core/application.h"

extern ApplicationConfig configApp();

int main(int argc, const char* argv[])
{
    ApplicationConfig config = configApp();
    Application app;

    appStartup(&config, &app);
    appRun(&app);
    appShutdown(&app);

    return 0;
}

#endif