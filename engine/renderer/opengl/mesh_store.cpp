#include "mesh_store.h"

#include "mesh.h"
#include "renderer/renderer_memory.h"

struct {
  long *elems;
  mesh_buffer_gl *buffers;
  long count;
} __mesh_store;

bool mesh_store_gl_init(long count) {
  assert(count > 0);
  __mesh_store.elems = renderer_make_array<long>(count);
  __mesh_store.buffers = renderer_make_array<mesh_buffer_gl>(count);
  __mesh_store.count = count;
}

void mesh_store_gl_deinit() {

  if (__mesh_store.elems) {
    renderer_destroy_array(__mesh_store.elems, __mesh_store.count);
    __mesh_store.elems = nullptr;
  }

  if (__mesh_store.buffers) {
    renderer_destroy_array(__mesh_store.buffers, __mesh_store.count);
    __mesh_store.buffers = nullptr;
  }
}

void mesh_store_gl_store(uint32_t mesh_id, long elems, mesh_buffer_gl buffers) {
  mesh_id--;
  assert(mesh_id < __mesh_store.count);
  __mesh_store.elems[mesh_id] = elems;
  __mesh_store.buffers[mesh_id] = buffers;
}

mesh_buffer_gl mesh_store_gl_get_buffers(uint32_t mesh_id) {
  mesh_id--;
  assert(mesh_id < __mesh_store.count);
  return __mesh_store.buffers[mesh_id];
}

void mesh_store_gl_get_elements(const uint32_t *mesh_ids, long *elem_counts,
                                long size) {
  for (long i = 0; i < size; i++) {
    uint32_t mesh_id = mesh_ids[i] - 1;
    assert(mesh_id < __mesh_store.count);
    elem_counts[i] = __mesh_store.elems[mesh_id];
  }
}
