#ifndef VULKAN_VERTEX_BUFFER_H
#define VULKAN_VERTEX_BUFFER_H

#include "vulkan_def.h"
#include "renderer/vertex_buffer.h"

void createVertexBuffer(VulkanContext* context, Vertex* vertices, u32 numVertices);

#endif