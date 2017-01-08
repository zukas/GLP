#ifndef MEMORY_DEVICE_H
#define MEMORY_DEVICE_H

#include "common_types.h"
#include "engine_info.h"

bool vk_memory_init(size_t host_stage_memory, size_t device_geom_memory,
                    size_t device_texture_memory);
void vk_memory_deinit();

VkMemoryBlockExt vk_memory_stage_blk_acquire(const VkBuffer buffer);
VkMemoryBlockExt vk_memory_gpu_blk_acquire(const VkBuffer buffer);

void vk_memory_blk_release(VkMemoryBlockExt blk);

#endif // MEMORY_DEVICE_H
