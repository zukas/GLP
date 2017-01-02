#ifndef PROGRAM_OPENGL_H
#define PROGRAM_OPENGL_H

#include "glm/glm.hpp"

#include <cstdint>

uint32_t compile_shader_vertex(const char **sources, int32_t count);
uint32_t compile_shader_fragment(const char **sources, int32_t count);
uint32_t compile_shader_geometry(const char **sources, int32_t count);
uint32_t compile_shader_tessellation(const char **sources, int32_t count);
void destroy_shader(uint32_t shader_id);

uint32_t complie_program(const uint32_t *schaders, int32_t count);
void destroy_program(uint32_t program_id);

int32_t uniform_count(uint32_t program_id);
int32_t resolve_uniform(uint32_t program_id, const char *uniform_name);

void use_program(uint32_t program_id);

void set_uniform(int32_t uniform_id, int32_t value);
void set_uniform(int32_t uniform_id, uint32_t value);
void set_uniform(int32_t uniform_id, float value);
void set_uniform(int32_t uniform_id, const glm::vec2 &value);
void set_uniform(int32_t uniform_id, const glm::vec3 &value);
void set_uniform(int32_t uniform_id, const glm::vec4 &value);
void set_uniform(int32_t uniform_id, const glm::mat3 &value);
void set_uniform(int32_t uniform_id, const glm::mat4 &value);

#endif // PROGRAM_OPENGL_H
