#include "projection.h"

#include "glm/gtc/matrix_transform.hpp"

static projection __projection;

static glm::mat4 __persp;
static glm::mat4 __ortho;

void projection_set(const projection &proj) {
  __projection = proj;
  __persp = glm::perspectiveFov(__projection.fov, __projection.width,
                                __projection.heaight, __projection.near,
                                __projection.far);
  __ortho = glm::ortho(0.f, 0.f, __projection.width, __projection.heaight);
}

projection projection_get() { return __projection; }

glm::mat4 projection_perspective() { return __persp; }

glm::mat4 projection_orthographic() { return __ortho; }
