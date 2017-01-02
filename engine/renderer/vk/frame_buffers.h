#ifndef FRAME_BUFFERS_H
#define FRAME_BUFFERS_H

#include "engine_info.h"

bool vk_frame_buffers_init(size_t count);
void vk_frame_buffers_deinit();

bool vk_frame_buffers_create(const pipline_description &pipeline_desc,
                             const VkPipelineExt *pipeline,
                             VkFramebuffersExt *frame_buffers);

void vk_frame_buffers_destroy(const VkFramebuffersExt *frame_buffers);

#endif // FRAME_BUFFERS_H
