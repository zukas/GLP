#ifndef TYPES_H
#define TYPES_H

#include "engine_info.h"
#include "renderer/vk/common_types.h"

struct VkPipelineExt_T {
  VkPipeline pipeline;
  VkPipelineLayout layout;
  VkRenderPass render_pass;
} PACKED;

struct VkFramebuffersExt_T {
  VkFramebuffer *frame_buffers;
  size_t size;
};

struct VkCommandBuffersExt_T {
  VkCommandBuffer *command_buffers;
  size_t size;
};

struct VkDataBufferExt_T {
  VkBuffer buffer;
  VkMemoryBlockExt block;
};

#endif // TYPES_H
