#include "program.h"

#include "io/io.h"

#include "gl_headers.h"

void compile_generic_shader(uint32_t shader_id, const char **sources,
                            int32_t count) {
  GLint status{GL_FALSE};
  glShaderSource(shader_id, count, sources, nullptr);
  glCompileShader(shader_id);
  glGetShaderiv(shader_id, GL_COMPILE_STATUS, &status);
  if (!status) {
    int logLength{0};
    glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0) {
      char error_str[4096]{0};
      glGetShaderInfoLog(shader_id, 4096, nullptr, error_str);
      error("Failed to compile shader - error: %s", error_str);
    }
  }
}

uint32_t compile_shader_vertex(const char **sources, int32_t count) {
  uint32_t shader_id = glCreateShader(GL_VERTEX_SHADER);
  compile_generic_shader(shader_id, sources, count);
  return shader_id;
}

uint32_t compile_shader_fragment(const char **sources, int32_t count) {
  uint32_t shader_id = glCreateShader(GL_FRAGMENT_SHADER);
  compile_generic_shader(shader_id, sources, count);
  return shader_id;
}

uint32_t compile_shader_geometry(const char **sources, int32_t count) {
  uint32_t shader_id = glCreateShader(GL_GEOMETRY_SHADER);
  compile_generic_shader(shader_id, sources, count);
  return shader_id;
}

uint32_t compile_shader_tessellation(const char **sources, int32_t count) {
  uint32_t shader_id = glCreateShader(GL_TESS_CONTROL_SHADER);
  compile_generic_shader(shader_id, sources, count);
  return shader_id;
}

void destroy_shader(uint32_t shader_id) { glDeleteShader(shader_id); }

uint32_t complie_program(const uint32_t *schaders, int32_t count) {
  uint32_t program_id = glCreateProgram();
  for (int i = 0; i < count; i++) {
    glAttachShader(program_id, schaders[i]);
  }
  GLint status{GL_FALSE};
  glLinkProgram(program_id);
  glGetProgramiv(program_id, GL_LINK_STATUS, &status);
  if (!status) {
    int logLength{0};
    glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0) {
      char error_str[2048]{0};
      glGetProgramInfoLog(program_id, logLength, nullptr, error_str);
      error("Program link error: %s\n", error_str);
    }
  }

  return program_id;
}

void destroy_program(uint32_t program_id) { glDeleteProgram(program_id); }

int32_t uniform_count(uint32_t program_id) {
  GLint count;
  glGetProgramiv(program_id, GL_ACTIVE_UNIFORMS, &count);
  return count;
}

int32_t resolve_uniform(uint32_t program_id, const char *uniform_name) {
  return glGetUniformLocation(program_id, uniform_name);
}

void use_program(uint32_t program_id) { glUseProgram(program_id); }

void set_uniform(int32_t uniform_id, int32_t value) {
  glUniform1i(uniform_id, value);
}

void set_uniform(int32_t uniform_id, uint32_t value) {
  glUniform1ui(uniform_id, value);
}

void set_uniform(int32_t uniform_id, float value) {
  glUniform1f(uniform_id, value);
}

void set_uniform(int32_t uniform_id, const glm::vec2 &value) {
  glUniform2fv(uniform_id, 1, &value[0]);
}

void set_uniform(int32_t uniform_id, const glm::vec3 &value) {
  glUniform3fv(uniform_id, 1, &value[0]);
}

void set_uniform(int32_t uniform_id, const glm::vec4 &value) {
  glUniform4fv(uniform_id, 1, &value[0]);
}

void set_uniform(int32_t uniform_id, const glm::mat3 &value) {
  glUniformMatrix3fv(uniform_id, 1, GL_FALSE, &value[0][0]);
}

void set_uniform(int32_t uniform_id, const glm::mat4 &value) {
  glUniformMatrix4fv(uniform_id, 1, GL_FALSE, &value[0][0]);
}
