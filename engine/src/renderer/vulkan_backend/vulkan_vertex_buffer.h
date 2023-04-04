#ifndef VULKAN_VERTEX_BUFFER_H
#define VULKAN_VERTEX_BUFFER_H

#include "vulkan_def.h"
#include "renderer/renderer.h"

void createVertexBuffer(VulkanContext* context, Vertex* vertices, u32 numVertices);

#endif