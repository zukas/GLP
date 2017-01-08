#include "geometry.h"

void generate_bound_plane3(const bound_plane3 &plane, vertex_3d_uv *vertexes,
                           u_int32_t *indexes, uint32_t subparts) {}

void generate_sphere3(const sphere3 &sphare, vertex_3d_uv *vertexes,
                      u_int32_t *indexes, uint32_t subparts) {

  float _units = float(int32_t(subparts));
  uint32_t index = 0;
  uint32_t size = subparts * subparts + subparts;

  for (float i = 0; i <= subparts; ++i) {
    // V texture coordinate.
    float V = i / _units;
    float phi = V * float(M_PI);

    for (float j = 0; j <= subparts; ++j) {
      // U texture coordinate.
      float U = j / _units;
      float theta = U * float(M_PI) * 2;

      float X = glm::cos(theta) * glm::sin(phi);
      float Y = glm::cos(phi);
      float Z = glm::sin(theta) * glm::sin(phi);

      vertexes[index].pos = glm::vec3{X, Y, Z};
      vertexes[index].norm = glm::normalize(glm::vec3{X, Y, Z});
      vertexes[index].uv = glm::vec2{U, V};
    }
  }

  for (index = 0; index < size; ++index) {
    indexes[index * 6] = index;
    indexes[index * 6 + 1] = index + subparts + 1;
    indexes[index * 6 + 2] = index + subparts;
    indexes[index * 6 + 3] = index + subparts + 1;
    indexes[index * 6 + 4] = index;
    indexes[index * 6 + 5] = index + 1;
  }
}

void generate_cube3(const cube3 &cube, vertex_3d_uv *vertexes,
                    uint32_t *indexes, uint32_t subparts) {}
