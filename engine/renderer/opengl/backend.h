#ifndef BACKEND_OPENGL_H
#define BACKEND_OPENGL_H

#include "renderer/gpu_primitives.h"
#include <cstddef>
#include <cstdint>

extern "C" {
bool init();
void deinit();
void render();
void init_mesh_store(size_t count);
uint32_t create_mesh(vertex *vertexes, long v_size, uint32_t *indexes,
                     long i_size);
void destroy_mesh(uint32_t mesh_id);
}

#endif // BACKEND_OPENGL_H
