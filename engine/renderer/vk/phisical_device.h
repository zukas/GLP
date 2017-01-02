#ifndef PHISICAL_DEVICE_H
#define PHISICAL_DEVICE_H

#include "engine_info.h"
#include "vulkan/vulkan.h"

bool vk_ph_device_init(const device_description &desc);
void vk_ph_device_deinit();

VkPhysicalDevice vk_ph_device_get();

swap_chain_description vk_ph_device_swap_chain_desc_get();

queue_family_description vk_ph_device_queue_family_desc_get();

#endif // PHISICAL_DEVICE_H
