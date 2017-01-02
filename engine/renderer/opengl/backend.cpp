#include "backend.h"

#include "engine_global.h"
#include "io/io.h"
#include "mesh.h"
#include "mesh_store.h"
#include "renderer/gpu_primitives.h"
#include "renderer/primitives/viewport.h"
#include "renderer/renderer_memory.h"

#include "gl_headers.h"
#include <GLFW/glfw3.h>

struct {
  GLFWwindow *window{nullptr};
} context;

viewport init() {

  context.window = static_cast<GLFWwindow *>(viewport_create_gl());

  glewExperimental = true;
  if (glewInit() != GLEW_OK) {
    error("Failed to initialize glfw");
    viewport_destroy(context.window);
    context.window = nullptr;
    return nullptr;
  }
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
  if (!GLEW_VERSION_4_5) {
#pragma GCC diagnostic pop
    error("OpenGL 4.5 is not supported");
    viewport_destroy(context.window);
    context.window = nullptr;
    return nullptr;
  }

  glfwSetInputMode(context.window, GLFW_STICKY_KEYS, GL_TRUE);

  glClearColor(0.2f, 0.3f, 0.5f, 1.0f);
  glClearDepth(1.0);
  glDepthFunc(GL_LESS);
  glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
  glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
  glEnable(GL_LINE_SMOOTH);
  glEnable(GL_POLYGON_SMOOTH);
  glEnable(GL_MULTISAMPLE);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

  return context.window;
}

void deinit() {

  if (context.window) {
    viewport_destroy(context.window);
    context.window = nullptr;
  }
}

void begin_frame() {
  log("Rendering");
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void end_frame() { glfwSwapBuffers(context.window); }

void init_mesh_store(long count) { mesh_store_gl_init(count); }

uint32_t create_mesh(vertex *vertexes, long v_size, uint32_t *indexes,
                     long i_size) {
  return create_mesh_gl(vertexes, v_size, indexes, i_size);
}

void destroy_mesh(uint32_t mesh_id) { destroy_mesh_gl(mesh_id); }

void render_meshes(const uint32_t *mesh_ids, long size) {
  render_meshes_gl(mesh_ids, size);
}
