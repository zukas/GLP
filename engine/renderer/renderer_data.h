#ifndef GPU_PRIMITIVES_H
#define GPU_PRIMITIVES_H

#include "glm/glm.hpp"

struct vertex {
  glm::vec3 pos;
  glm::vec3 norm;
  glm::vec2 uv;
};

#endif // GPU_PRIMITIVES_H
