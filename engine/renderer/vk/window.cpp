#include "window.h"

#include "GLFW/glfw3.h"
#include "instance.h"

struct {
  GLFWwindow *window{nullptr};
  VkSurfaceKHR surface{VK_NULL_HANDLE};
} __context;

bool vk_window_init(const char *name) {
  bool status = glfwInit();
  if (status) {
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    auto monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    __context.window =
        glfwCreateWindow(mode->width, mode->height, name, monitor, nullptr);
    status = __context.window != nullptr;
  }
  return status;
}

void vk_window_deinit() {
  if (__context.window) {
    glfwDestroyWindow(__context.window);
    __context.window = nullptr;
  }
  glfwTerminate();
}

GLFWwindow *vk_window_get() { return __context.window; }

window_size vk_window_size() {
  const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
  return window_size{static_cast<uint32_t>(mode->width),
                     static_cast<uint32_t>(mode->height)};
}

bool vk_window_surface_init() {
  return glfwCreateWindowSurface(vk_instance_get(), __context.window, nullptr,
                                 &__context.surface) == VK_SUCCESS;
}

void vk_window_surface_deinit() {
  vkDestroySurfaceKHR(vk_instance_get(), __context.surface, nullptr);
}

VkSurfaceKHR vk_window_surface_get() { return __context.surface; }
