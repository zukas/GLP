#ifndef BACKEND_OPENGL_H
#define BACKEND_OPENGL_H

#include "renderer/gpu_primitives.h"

extern "C" {
viewport init();
void deinit();
void begin_frame();
void end_frame();
void init_mesh_store(long count);
uint32_t create_mesh(vertex *vertexes, long v_size, uint32_t *indexes,
                     long i_size);
void destroy_mesh(uint32_t mesh_id);
void render_meshes(const uint32_t *mesh_ids, long size);
}

#endif // BACKEND_OPENGL_H
