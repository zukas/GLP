#ifndef TYPES_H
#define TYPES_H

#include "engine_info.h"

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

#endif // TYPES_H
