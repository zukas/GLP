#ifndef TYPES_H
#define TYPES_H

#include "renderer/renderer_data.h"

struct plane3 {
  glm::vec3 pos;
  glm::vec3 norm;
};

struct bound_plane3 {
  glm::vec3 pos;
  glm::vec3 norm;
  float width;
  float height;
};

struct sphere3 {
  glm::vec3 pos;
  float radius;
};

struct cube3 {
  glm::vec3 pos;
  float width;
  float height;
  float depth;
};

struct ray3 {
  glm::vec3 pos;
  glm::vec3 dir;
};

void generate_bound_plane3(const bound_plane3 &plane, vertex *vertexes,
                           u_int32_t *indexes, uint32_t subparts);

void generate_sphere3(const sphere3 &sphare, vertex *vertexes,
                      u_int32_t *indexes, uint32_t subparts);

constexpr uint32_t vertex_size_sphere3(uint32_t subparts) {
  return (subparts + 1) * (subparts + 1);
}

constexpr uint32_t index_size_sphere3(uint32_t subparts) {
  return (subparts * subparts + subparts) * 6;
}

void generate_cube3(const cube3 &cube, vertex *vertexes, uint32_t *indexes,
                    uint32_t subparts);

#endif // TYPES_H
