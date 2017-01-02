#ifndef RENDER_H
#define RENDER_H

#include "engine_info.h"
#include "renderer_data.h"

bool renderer_init(const engine_description &desc);
void renderer_deinit();

#endif // RENDER_H
