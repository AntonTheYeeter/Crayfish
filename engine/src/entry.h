#ifndef ENTRY_H
#define ENTRY_H

#include "core/application.h"

extern ApplicationConfig configApp();

extern void start();
extern void update(f32 delta);

int main(int argc, const char* argv[])
{
    ApplicationConfig config = configApp();

    appStartup(&config, start, update);
    appRun();
    appShutdown();

    return 0;
}

#endif