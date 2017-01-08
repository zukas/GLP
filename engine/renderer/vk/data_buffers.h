#ifndef VERTEX_H
#define VERTEX_H

#include "engine_info.h"
#include "renderer/renderer_data.h"

bool vk_data_buffers_init(size_t size);
void vk_data_buffers_deinit();

VkDataBufferExt vk_data_buffer_create(void *data, size_t size);
void vk_data_buffer_destroy(const VkDataBufferExt buffer);

#endif // VERTEX_H
