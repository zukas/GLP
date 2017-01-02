#ifndef MESH_STORE_H
#define MESH_STORE_H

#include "mesh.h"

#include <cstddef>
#include <cstdint>

typedef struct { uint32_t data[2]; } mesh_buffer_gl;

bool mesh_store_gl_init(long count);
void mesh_store_gl_deinit();

void mesh_store_gl_store(uint32_t mesh_id, long elems, mesh_buffer_gl buffers);
mesh_buffer_gl mesh_store_gl_get_buffers(uint32_t mesh_id);
void mesh_store_gl_get_elements(const uint32_t *mesh_ids, long *elem_counts,
                                long size);

#endif // MESH_STORE_H
