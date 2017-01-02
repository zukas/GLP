#include "logical_device.h"

#include "constant.h"
#include "phisical_device.h"

struct {
  VkDevice device{VK_NULL_HANDLE};
} __context;

bool vk_lg_device_init() {
  queue_family_description queue_indexes = vk_ph_device_queue_family_desc_get();

  uint32_t queue_count = 1;
  float priotity[]{1.0};
  VkDeviceQueueCreateInfo queue_info[2]{};

  queue_info[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  queue_info[0].queueFamilyIndex = queue_indexes.graphics_queue_family;
  queue_info[0].queueCount = 1;
  queue_info[0].pQueuePriorities = priotity;

  if (queue_indexes.graphics_queue_family !=
      queue_indexes.present_queue_family) {
    queue_info[1].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_info[1].queueFamilyIndex = queue_indexes.present_queue_family;
    queue_info[1].queueCount = 1;
    queue_info[1].pQueuePriorities = priotity;
    queue_count = 2;
  }

  VkPhysicalDeviceFeatures features = {};

  VkDeviceCreateInfo device_info = {};
  device_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  device_info.pQueueCreateInfos = queue_info;
  device_info.queueCreateInfoCount = queue_count;

  device_info.pEnabledFeatures = &features;

  device_info.enabledExtensionCount = device_extension_size;
  device_info.ppEnabledExtensionNames = device_extensions;

#ifdef NDEBUG
  device_info.enabledLayerCount = 0;
#else
  device_info.enabledLayerCount = validation_layer_size;
  device_info.ppEnabledLayerNames = validation_layers;
#endif
  VkResult res = vkCreateDevice(vk_ph_device_get(), &device_info, nullptr,
                                &__context.device);

  return res == VK_SUCCESS;
}

void vk_lg_device_deinit() {
  vkDestroyDevice(__context.device, nullptr);
  __context.device = nullptr;
}

VkDevice vk_lg_device_get() { return __context.device; }
