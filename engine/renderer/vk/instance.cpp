#include "instance.h"

#include "GLFW/glfw3.h"
#include "constant.h"
#include "utils/fixed_array.h"

#include <cassert>
#include <cstdio>
#include <cstring>

static VkInstance __instance{VK_NULL_HANDLE};

#ifndef NDEBUG
static VkDebugReportCallbackEXT __debug{VK_NULL_HANDLE};

static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
    VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT, uint64_t, size_t,
    int32_t code, const char *layerPrefix, const char *msg, void *) {

  if (flags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT) {
    printf("VINFO: [%s] Code %d : %s\n", layerPrefix, code, msg);
  } else if (flags & VK_DEBUG_REPORT_WARNING_BIT_EXT) {
    printf("VWARNING: [%s] Code %d : %s\n", layerPrefix, code, msg);

  } else if (flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT) {
    printf("VPERF: [%s] Code %d : %s\n", layerPrefix, code, msg);

  } else if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT) {
    printf("VERROR: [%s] Code %d : %s\n", layerPrefix, code, msg);

  } else if (flags & VK_DEBUG_REPORT_DEBUG_BIT_EXT) {
    printf("VDEBUG: [%s] Code %d : %s\n", layerPrefix, code, msg);

  } else {
    printf("VINFO: [%s] Code %d : %s\n", layerPrefix, code, msg);
  }

  fflush(stdout);
  return VK_FALSE;
}

VkResult CreateDebugReportCallbackEXT(
    VkInstance instance, const VkDebugReportCallbackCreateInfoEXT *pCreateInfo,
    const VkAllocationCallbacks *pAllocator,
    VkDebugReportCallbackEXT *pCallback) {
  PFN_vkCreateDebugReportCallbackEXT func =
      reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>(
          vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT"));
  if (func != nullptr) {
    return func(instance, pCreateInfo, pAllocator, pCallback);
  } else {
    return VK_ERROR_EXTENSION_NOT_PRESENT;
  }
}

void DestroyDebugReportCallbackEXT(VkInstance instance,
                                   VkDebugReportCallbackEXT callback,
                                   const VkAllocationCallbacks *pAllocator) {
  PFN_vkDestroyDebugReportCallbackEXT func =
      reinterpret_cast<PFN_vkDestroyDebugReportCallbackEXT>(
          vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT"));
  if (func != nullptr) {
    func(instance, callback, pAllocator);
  }
}
#endif

fixed_array<const char *> load_extensions() {
  uint32_t glfw_ext_count = 0;
  const char **glfw_ext;
  glfw_ext = glfwGetRequiredInstanceExtensions(&glfw_ext_count);

#ifndef NDEBUG
  fixed_array<const char *> ext(glfw_ext_count + 1);
#else
  fixed_array<const char *> ext(glfw_ext_count);

#endif
  int ext_count = static_cast<int>(glfw_ext_count);
  for (int i = 0; i < ext_count; i++) {
    ext[i] = glfw_ext[i];
  }

#ifndef NDEBUG
  ext[ext_count] = VK_EXT_DEBUG_REPORT_EXTENSION_NAME;
#endif
  return ext;
}

bool validation_enabled() {
#ifndef NDEBUG
  uint32_t layer_count;
  vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

  fixed_array<VkLayerProperties> available(layer_count);
  vkEnumerateInstanceLayerProperties(&layer_count, available.data());

  int validation_layer_count = static_cast<int>(validation_layer_size);
  int current_layer_count = static_cast<int>(layer_count);

  for (int i = 0; i < validation_layer_count; i++) {
    const char *name = validation_layers[i];
    for (int j = current_layer_count - 1; j >= 0; j--) {
      if (strcmp(name, available[j].layerName) == 0) {
        return true;
      }
    }
  }

  return false;
#else
  return true;
#endif
}

bool vk_instance_init(const app_description &desc) {

  bool status = validation_enabled();
  if (status) {
    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = desc.app_name;
    appInfo.applicationVersion =
        VK_MAKE_VERSION(desc.version_maj, desc.version_min, desc.version_patch);
    appInfo.pEngineName = ENGINE_NAME;
    appInfo.engineVersion = VK_MAKE_VERSION(
        ENGINE_VERSION_MAJ, ENGINE_VERSION_MIN, ENGINE_VERSION_PATCH);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    fixed_array<const char *> ext = load_extensions();
    createInfo.enabledExtensionCount = ext.size();
    createInfo.ppEnabledExtensionNames = ext.data();

#ifndef NDEBUG
    createInfo.ppEnabledLayerNames = validation_layers;
    createInfo.enabledLayerCount = validation_layer_size;
#else
    createInfo.enabledLayerCount = 0;
#endif
    status = vkCreateInstance(&createInfo, nullptr, &__instance) == VK_SUCCESS;
  }
#ifndef NDEBUG
  if (status) {

    VkDebugReportCallbackCreateInfoEXT debug_info = {};
    debug_info.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
    debug_info.flags =
        VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
    debug_info.pfnCallback = debug_callback;

    status = CreateDebugReportCallbackEXT(__instance, &debug_info, nullptr,
                                          &__debug) == VK_SUCCESS;
  }
#endif

  return status;
}

void vk_instance_deinit() {

#ifndef NDEBUG
  DestroyDebugReportCallbackEXT(__instance, __debug, nullptr);
  __debug = VK_NULL_HANDLE;
#endif
  vkDestroyInstance(__instance, nullptr);
  __instance = VK_NULL_HANDLE;
}

VkInstance vk_instance_get() { return __instance; }
