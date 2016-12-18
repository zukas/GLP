#include "mesh_store.h"

#include "mesh.h"
#include "renderer/renderer_memory.h"

#include <cstring>

mesh_store::mesh_store() : m_buffers(nullptr), m_count(0) {}

mesh_store::mesh_store(size_t count)
    : m_buffers(static_cast<mesh_buffer_gl *>(
          renderer_mem_alloc(sizeof(mesh_buffer_gl) * count))),
      m_count(count) {
  memset(m_buffers, 0, sizeof(mesh_buffer_gl) * count);
}

mesh_store::~mesh_store() {
  if (m_buffers) {
    renderer_mem_free(m_buffers, sizeof(mesh_buffer_gl) * m_count);
    m_buffers = nullptr;
  }
}

void mesh_store::store(uint32_t mesh_id, mesh_buffer_gl buffer) {
  assert(mesh_id < m_count);
  m_buffers[mesh_id] = buffer;
}

mesh_buffer_gl mesh_store::get(uint32_t mesh_id) {
  assert(mesh_id < m_count);
  return m_buffers[mesh_id];
}
