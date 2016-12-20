#include "backend.h"

#include "engine_global.h"
#include "io/io.h"
#include "mesh.h"
#include "mesh_store.h"
#include "renderer/gpu_primitives.h"
#include "renderer/renderer_memory.h"

#include "gl_headers.h"
#include <GLFW/glfw3.h>

static struct context_gl { GLFWwindow *window{nullptr}; } * context{nullptr};

bool init() {
  context = renderer_new<context_gl>();
  if (!glfwInit()) {
    error("Failed to initialize glfw");
    renderer_delete(context);
    context = nullptr;
    return false;
  }
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  auto monitor = glfwGetPrimaryMonitor();
  const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
  context->window = glfwCreateWindow(mode->width, mode->height, WINDOW_TITLE,
                                     monitor, nullptr);
  glfwMakeContextCurrent(context->window);

  glewExperimental = true;
  if (glewInit() != GLEW_OK) {
    error("Failed to initialize glfw");
    renderer_delete(context);
    context = nullptr;
    return false;
  }
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
  if (!GLEW_VERSION_4_5) {
#pragma GCC diagnostic pop
    error("OpenGL 4.5 is not supported");
    renderer_delete(context);
    context = nullptr;
    return false;
  }

  glfwSetInputMode(context->window, GLFW_STICKY_KEYS, GL_TRUE);
  glfwSetCursorPos(context->window, mode->width / 2, mode->height / 2);

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

  return true;
}

void deinit() {

  if (context) {
    if (context->window) {
      glfwDestroyWindow(context->window);
      context->window = nullptr;
    }

    renderer_delete(context);
  }
  glfwTerminate();
}

void begin_frame() {
  log("Rendering");
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void end_frame() { glfwSwapBuffers(context->window); }

void init_mesh_store(size_t count) { init_mesh_store_gl(count); }

uint32_t create_mesh(vertex *vertexes, long v_size, uint32_t *indexes,
                     long i_size) {
  return create_mesh_gl(vertexes, v_size, indexes, i_size);
}

void destroy_mesh(uint32_t mesh_id) { destroy_mesh_gl(mesh_id); }

void render_mesh(uint32_t mesh_id) { render_mesh_gl(mesh_id); }
