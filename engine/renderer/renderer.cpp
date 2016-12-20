#include "renderer.h"

#include "engine_global.h"
#include "renderer_memory.h"

#include <cstring>

Render::Render() : m_mesh_store(nullptr), m_mesh_store_size(0) {
  renderer_mem_init();
}

Render::~Render() { renderer_mem_deinit(); }

void Render::deinit() { m_backend.deinit(); }

bool Render::initOpenGL() {
  m_backend.load(Backend::OpengGL);
  return m_backend.init();
}

bool Render::initVulkan() {
  m_backend.load(Backend::Vulkan);
  return m_backend.init();
}

void Render::render() {
  m_backend.begin_frame();
  const long m_count = static_cast<long>(m_mesh_store_size);
  for (long i = 0; i < m_count; ++i) {
    m_backend.render_mesh(m_mesh_store[i]);
  }
  m_backend.end_frame();
}

void Render::init_mesh_store(size_t count) { m_backend.init_mesh_store(count); }

uint32_t Render::create_mesh(vertex *vertexes, long v_size, uint32_t *indexes,
                             long i_size) {
  m_backend.create_mesh(vertexes, v_size, indexes, i_size);
}

void Render::destroy_mesh(uint32_t mesh_id) { m_backend.destroy_mesh(mesh_id); }

void Render::set_mesh_store(uint32_t *mesh_ids, size_t count) {
  if (count > m_mesh_store_size) {
    uint32_t *store =
        static_cast<uint32_t *>(renderer_mem_alloc(sizeof(uint32_t) * count));
    memcpy(store, mesh_ids, sizeof(uint32_t) * count);
    uint32_t *old = m_mesh_store;
    size_t old_size = m_mesh_store_size;
    m_mesh_store = store;
    m_mesh_store_size = count;
    if (old != nullptr) {
      renderer_mem_free(old, sizeof(uint32_t) * old_size);
    }
  } else {
    memcpy(m_mesh_store, mesh_ids, sizeof(uint32_t) * count);
  }
}
