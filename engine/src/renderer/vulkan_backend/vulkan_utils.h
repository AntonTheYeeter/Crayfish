#ifndef VULKAN_UTILS_H
#define VULKAN_UTILS_H

#include "def.h"

#include <vulkan/vulkan.h>

const char* vulkanResultToString(VkResult res);
b8 vulkanResultIsError(VkResult res);

#endif