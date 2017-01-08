#ifndef GPU_PRIMITIVES_H
#define GPU_PRIMITIVES_H

#include "glm/glm.hpp"

struct vertex_2d_uv {
  glm::vec2 pos;
  glm::vec2 uv;
};

struct vertex_2d_color {
  glm::vec2 pos;
  glm::vec3 color;
};

struct vertex_3d_uv {
  glm::vec3 pos;
  glm::vec3 norm;
  glm::vec2 uv;
};

struct vertex_3d_color {
  glm::vec3 pos;
  glm::vec3 norm;
  glm::vec3 color;
};

#endif // GPU_PRIMITIVES_H
