#ifndef GRAPHICS_PIPLINE_H
#define GRAPHICS_PIPLINE_H

#include "engine_info.h"

VkShaderModule vk_shader_create(const char *source, size_t length);
void vk_shader_destroy(VkShaderModule module);

bool vk_pipeline_init(size_t count);
void vk_pipeline_deinit();

VkPipelineExt vk_pipeline_create(const pipline_description &desc);

void vk_pipline_destroy(const VkPipelineExt pipeline);

#endif // GRAPHICS_PIPLINE_H
