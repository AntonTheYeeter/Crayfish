#ifndef ENTRY_H
#define ENTRY_H

#include "core/application.h"

extern ApplicationConfig configApp();

int main(int argc, const char* argv[])
{
    ApplicationConfig config = configApp();
    Application app;

    appStartup(&config);
    appRun();
    appShutdown();

    return 0;
}

#endif