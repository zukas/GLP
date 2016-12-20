#include "renderer/geometry/types.h"
#include "renderer/renderer.h"
#include "runtime/runtime.h"

int main() {
  Runtime e;

  constexpr uint32_t v_size = vertex_size_sphere3(32);
  constexpr uint32_t i_size = index_size_sphere3(32);
  vertex vertexes[v_size];
  uint32_t indexes[i_size];
  generate_sphere3(sphere3{glm::vec3{0.f, 0.f, 0.f}, 3.f}, vertexes, indexes,
                   32);
  G_Renderer::get()->init_mesh_store(16);
  uint32_t mesh_id =
      G_Renderer::get()->create_mesh(vertexes, v_size, indexes, i_size);
  G_Renderer::get()->set_mesh_store(&mesh_id, 1);
  e.run();
}
