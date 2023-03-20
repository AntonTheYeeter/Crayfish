#ifndef VULKAN_SYNC_OBJECTS_H
#define VULKAN_SYNC_OBJECTS_H

#include "vulkan_def.h"

b8 createSyncObjects(VulkanContext* context);
void destroySyncObjects(VulkanContext* context);

#endif