#ifndef ENGINE_INFO_H
#define ENGINE_INFO_H

#define ENGINE_NAME "GLP Realtime"
#define ENGINE_VERSION_MAJ 1
#define ENGINE_VERSION_MIN 0
#define ENGINE_VERSION_PATCH 0

#ifdef __GNUG__
#define PACKED __attribute__((packed))
#define NORETURN __attribute__((noreturn))
#define FORCE_INLINE __attribute__((always_inline))
#endif

#define GLPHANDLE(type) typedef struct type##_T *type

#include "vulkan/vulkan.h"
#include <cstdint>

GLPHANDLE(VkPipelineExt);
GLPHANDLE(VkFramebuffersExt);

struct app_description {
  const char *app_name;
  uint32_t version_maj;
  uint32_t version_min;
  uint32_t version_patch;
};

struct device_description {};

struct window_size {
  uint32_t width;
  uint32_t height;
};

struct queue_family_description {
  uint32_t present_queue_family;
  uint32_t graphics_queue_family;
};

struct swap_chain_description {
  VkSurfaceCapabilitiesKHR capabilities;
  VkExtent2D extent;
  VkSurfaceFormatKHR format;
  VkPresentModeKHR mode;
} PACKED;

struct shader_description {
  VkShaderModule module;
  VkShaderStageFlagBits type;
};

struct pipline_description {
  shader_description shaders[4];
  uint32_t size;
};

struct engine_description {
  app_description app;
  device_description device;
};

#endif // ENGINE_INFO_H
