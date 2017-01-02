#ifndef PROJECTION_H
#define PROJECTION_H

#include "glm/glm.hpp"

struct projection {
  float fov;
  float width;
  float heaight;
  float near;
  float far;
};

void projection_set(const projection &proj);
projection projection_get();

glm::mat4 projection_perspective();
glm::mat4 projection_orthographic();

#endif // PROJECTION_H
