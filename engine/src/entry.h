#include "core/application.h"

extern CFAPI Application* createApp();

int main(void)
{
    Application* app = createApp();
    runApp(app);
    destroyApp(app);

    return 0;
}