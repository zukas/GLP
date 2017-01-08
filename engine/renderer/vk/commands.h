#ifndef COMMANDS_H
#define COMMANDS_H

#include "engine_info.h"

bool vk_cmd_pool_init(size_t count);
void vk_cmd_pool_deinit();

VkCommandBuffer vk_cmd_create();
void vk_cmd_destroy(VkCommandBuffer buffer);

VkCommandBuffersExt
vk_cmd_buffers_create(const VkFramebuffersExt frame_buffers);

void vk_cmd_buffers_destroy(const VkCommandBuffersExt cmd_buffers);

void vk_cmd_buffers_record(const VkPipelineExt pipeline,
                           const VkFramebuffersExt frame_buffers,
                           const VkCommandBuffersExt cmd_buffers,
                           const VkVertexBufferExt vertex_buffers);

#endif // COMMANDS_H
