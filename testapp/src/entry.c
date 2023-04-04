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
    Vertex vertices[] =
    {
        {{ 0.5,-0.5, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}},
        {{ 1.0, 0.5, 0.0f}, {0.0f, 1.0f, 0.0f, 1.0f}},
        {{ 0.0, 0.5, 0.0f}, {0.0f, 0.0f, 1.0f, 1.0f}},

        {{-0.5,-0.5, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}},
        {{ 0.0, 0.5, 0.0f}, {0.0f, 1.0f, 0.0f, 1.0f}},
        {{-1.0, 0.5, 0.0f}, {0.0f, 0.0f, 1.0f, 1.0f}}
    };

    u32 indices[] = 
    {
        0, 1, 2,
        3, 4, 5
    };

    Mesh mesh = 
    {
        .isVisible = TRUE,
        .numVertices = ARRAY_SIZE(vertices),
        .vertices = vertices,
        .numIndices = ARRAY_SIZE(indices),
        .indices = indices
    };

    addMesh(mesh);
}

void update(f32 delta)
{

}