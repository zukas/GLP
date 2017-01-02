#ifndef INSTANCE_H
#define INSTANCE_H

#include "engine_info.h"
#include "vulkan/vulkan.h"

bool vk_instance_init(const app_description &desc);
void vk_instance_deinit();

VkInstance vk_instance_get();

#endif // INSTANCE_H
