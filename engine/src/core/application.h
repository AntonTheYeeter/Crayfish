#ifndef APPLICATION_H
#define APPLICATION_H

#include "def.h"

typedef struct Application
{

} Application;

CFAPI Application* createApp();
void runApp(Application* app);
void destroyApp(Application* app);

#endif