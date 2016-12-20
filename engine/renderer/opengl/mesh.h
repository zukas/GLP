#ifndef MESH_OPENGL_H
#define MESH_OPENGL_H

#include "renderer/gpu_primitives.h"

#include <cstddef>
#include <cstdint>

typedef struct { uint32_t data[2]; } mesh_buffer_gl;

void init_mesh_store_gl(size_t count);

uint32_t create_mesh_gl(const vertex *__restrict vertexes, long v_size,
                        const uint32_t *__restrict indexes, long i_size);

void destroy_mesh_gl(uint32_t mesh);

void render_mesh_gl(uint32_t mesh_id);

#endif // MESH_OPENGL_H
