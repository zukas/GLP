#ifndef LOGICAL_DEVICE_H
#define LOGICAL_DEVICE_H

#include "engine_info.h"

bool vk_lg_device_init();
void vk_lg_device_deinit();

VkDevice vk_lg_device_get();

#endif // LOGICAL_DEVICE_H
