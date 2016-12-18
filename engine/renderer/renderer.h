#ifndef RENDER_H
#define RENDER_H

#include "backend.h"
#include "gpu_primitives.h"
#include <utils/global_instance.h>

class Render : public global_instance<Render> {
public:
  Render();
  ~Render();

  bool initOpenGL();
  bool initVulkan();
  void deinit();

  void render();

  void init_mesh_store(size_t count);
  uint32_t create_mesh(vertex *vertexes, long v_size, uint32_t *indexes,
                       long i_size);
  void destroy_mesh(uint32_t mesh_id);

private:
  Backend m_backend;
};

using G_Renderer = global_instance<Render>;

#endif // RENDER_H
