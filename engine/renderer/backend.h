#ifndef BACKEND_H
#define BACKEND_H

#include "gpu_primitives.h"
#include <cstddef>
#include <cstdint>

typedef bool (*init_fn)();
typedef void (*deinit_fn)();
typedef void (*render_fn)();
typedef void (*init_mesh_store_fn)(size_t count);
typedef uint32_t (*create_mesh_fn)(vertex *vertexes, long v_size,
                                   uint32_t *indexes, long i_size);
typedef void (*destroy_mesh_fn)(uint32_t mesh_id);

struct backend_mapper {
  void *handle;
  init_fn init;
  deinit_fn deinit;
  render_fn render;
  init_mesh_store_fn init_mesh_store;
  create_mesh_fn create_mesh;
  destroy_mesh_fn destroy_mesh;
};

class Backend {
public:
  enum BackendType { None = -1, OpengGL = 0, Vulkan = 1 };
  Backend();
  ~Backend();
  void load(BackendType type);
  BackendType type();

  bool init() { return m_mapper->init(); }
  void deinit() { m_mapper->deinit(); }
  void render() { m_mapper->render(); }
  void init_mesh_store(size_t count) { m_mapper->init_mesh_store(count); }
  uint32_t create_mesh(vertex *vertexes, long v_size, uint32_t *indexes,
                       long i_size) {
    m_mapper->create_mesh(vertexes, v_size, indexes, i_size);
  }
  void destroy_mesh(uint32_t mesh_id) { m_mapper->destroy_mesh(mesh_id); }

private:
  backend_mapper *m_mapper;
  BackendType m_type;
};

#endif // BACKEND_H