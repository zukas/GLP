#ifndef VERTEX_H
#define VERTEX_H

#include "engine_info.h"
#include "renderer/renderer_data.h"

bool vk_vertex_buffer_init(size_t size);
void vk_vertex_buffer_deinit();

VkVertexBufferExt vk_vertex_buffer_create(vertex_2d_color *data, size_t size);
void vk_vertex_buffer_destroy(const VkVertexBufferExt buffer);

#endif // VERTEX_H
