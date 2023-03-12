#include "core/application.h"

extern ApplicationConfig configApp();

int main(void)
{
    ApplicationConfig config = configApp();
    Application* app = createApp(&config);
    runApp(app);
    destroyApp(app);

    return 0;
}