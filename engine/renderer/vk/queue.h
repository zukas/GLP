#ifndef QUEUE_H
#define QUEUE_H

#include "engine_info.h"

bool vk_queue_init();
void vk_queue_deinit();

VkQueue vk_queue_graphics_get();
VkQueue vk_queue_present_get();

#endif // QUEUE_H
