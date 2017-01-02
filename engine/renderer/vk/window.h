#ifndef WINDOW_H
#define WINDOW_H

#include "GLFW/glfw3.h"
#include "engine_info.h"
#include "vulkan/vulkan.h"

bool vk_window_init(const char *name);
void vk_window_deinit();

GLFWwindow *vk_window_get();
window_size vk_window_size();

bool vk_window_surface_init();
void vk_window_surface_deinit();

VkSurfaceKHR vk_window_surface_get();

#endif // WINDOW_H
