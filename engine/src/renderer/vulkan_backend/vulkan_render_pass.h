#ifndef VULKAN_RENDER_PASS_H
#define VULKAN_RENDER_PASS_H

#include "vulkan_def.h"
#include "core/cf_memory.h"

void createRenderPass(VulkanContext* context);
void createFramebuffers(VulkanContext* context);

#endif