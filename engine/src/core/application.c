#include "application.h"

#include <stdlib.h>
#include <stdio.h>

void runApp(Application* app)
{
    while(TRUE)
    {
        printf("Hi\n");
    }
}

void destroyApp(Application* app)
{
    free(app);
}