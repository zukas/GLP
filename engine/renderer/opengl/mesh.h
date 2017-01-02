#ifndef MESH_OPENGL_H
#define MESH_OPENGL_H

#include "renderer/gpu_primitives.h"

#include <cstddef>
#include <cstdint>

uint32_t create_mesh_gl(const vertex *__restrict vertexes, long v_size,
                        const uint32_t *__restrict indexes, long i_size);

void destroy_mesh_gl(uint32_t mesh);

void render_meshes_gl(const uint32_t *mesh_ids, long size);

#endif // MESH_OPENGL_H
