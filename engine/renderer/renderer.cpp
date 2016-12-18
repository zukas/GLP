#include "renderer.h"

#include "engine_global.h"
#include "renderer_memory.h"

Render::Render() { renderer_mem_init(); }

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

void Render::render() { m_backend.render(); }

void Render::init_mesh_store(size_t count) { m_backend.init_mesh_store(count); }

uint32_t Render::create_mesh(vertex *vertexes, long v_size, uint32_t *indexes,
                             long i_size) {
  m_backend.create_mesh(vertexes, v_size, indexes, i_size);
}

void Render::destroy_mesh(uint32_t mesh_id) { m_backend.destroy_mesh(mesh_id); }
