#ifndef APPLICATION_H
#define APPLICATION_H

#include "def.h"

typedef struct Application
{
    u32 test;
} Application;

CFAPI void runApp(Application* app);
CFAPI void destroyApp(Application* app);

#endif