#ifndef BACKEND_VULKAN_H
#define BACKEND_VULKAN_H

#include "renderer/gpu_primitives.h"

class backend_vulkan {
public:
  backend_vulkan();
  ~backend_vulkan();
  bool init();
  void deinit();

  //  BackendType type() const;
  void render();
};

#endif // BACKEND_VULKAN_H
