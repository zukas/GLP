#ifndef RENDER_H
#define RENDER_H

#include "engine_info.h"
#include "renderer_data.h"

bool renderer_init(const engine_description &desc);
void renderer_deinit();

void renderer_execute(const VkCommandBuffersExt cmd_buffers);

#endif // RENDER_H
