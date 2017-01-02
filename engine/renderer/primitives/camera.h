#ifndef CAMERA_H
#define CAMERA_H

#include "glm/glm.hpp"

struct camera {
  glm::vec3 position;
  glm::vec3 direction;
  glm::vec3 up;
};

void camera_set(const camera &camera);
void camera_set_position(const glm::vec3 &position);
void camera_set_direction(const glm::vec3 &direction);
void camera_set_up(const glm::vec3 &up);

glm::mat4 camera_get_view();

#endif // CAMERA_H
