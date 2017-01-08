#ifndef COMMON_TYPES_H
#define COMMON_TYPES_H

#include "vulkan/vulkan.h"

enum VkMemoryAccess {
  VK_LOCAL = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
  VK_HOST_MODIFY = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
};

struct VkMemoryBlockExt {
  VkDeviceMemory memory;
  VkDeviceSize offset;
  VkDeviceSize size;
  void *state;
};

#endif // COMMON_TYPES_H
