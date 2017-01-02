#include "camera.h"

#include "glm/gtc/matrix_transform.hpp"

static camera __camera;

void camera_set(const camera &camera) { __camera = camera; }

void camera_set_position(const glm::vec3 &position) {
  __camera.position = position;
}

void camera_set_direction(const glm::vec3 &direction) {
  __camera.direction = direction;
}

void camera_set_up(const glm::vec3 &up) { __camera.up = up; }

glm::mat4 camera_get_view() {
  return glm::lookAt(__camera.position,
                     __camera.position + __camera.direction * 5.f, __camera.up);
}
