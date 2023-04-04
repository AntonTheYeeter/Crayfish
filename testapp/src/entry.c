#include <entry.h>
#include <core/application.h>
#include <core/logger.h>

ApplicationConfig configApp()
{
    ApplicationConfig config;
    config.windowPosX = 300;
    config.windowPosY = 300;
    config.windowWidth = 1280;
    config.windowHeight = 720;
    config.applicationName = "Crayfish Testapp";

    return config;
}

void start()
{
    Vertex triangle1Vertices[] =
    {
        {{ 0.0,-0.5, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}},
        {{ 0.5, 0.5, 0.0f}, {0.0f, 1.0f, 0.0f, 1.0f}},
        {{-0.5, 0.5, 0.0f}, {0.0f, 0.0f, 1.0f, 1.0f}}
    };

    Mesh triangle1 = 
    {
        .isVisible = TRUE,
        .numVertices = ARRAY_SIZE(triangle1Vertices),
        .vertices = triangle1Vertices
    };

    addMesh(triangle1);
}

void update(f32 delta)
{

}