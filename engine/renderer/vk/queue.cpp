#include "queue.h"

#include "logical_device.h"
#include "phisical_device.h"

struct {
  VkQueue present{VK_NULL_HANDLE};
  VkQueue graphics{VK_NULL_HANDLE};
} __context;

bool vk_queue_init() {
  queue_family_description queue_indexes = vk_ph_device_queue_family_desc_get();
  VkDevice device = vk_lg_device_get();
  vkGetDeviceQueue(device, queue_indexes.graphics_queue_family, 0,
                   &__context.graphics);
  vkGetDeviceQueue(device, queue_indexes.present_queue_family, 0,
                   &__context.present);

  return true;
}

void vk_queue_deinit() {
  __context.graphics = VK_NULL_HANDLE;
  __context.present = VK_NULL_HANDLE;
}

VkQueue vk_queue_graphics_get() { return __context.graphics; }

VkQueue vk_queue_present_get() { return __context.present; }
