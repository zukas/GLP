#include "phisical_device.h"

#include "constant.h"
#include "instance.h"
#include "utils/fixed_array.h"
#include "utils/math.h"
#include "window.h"

#include <cstring>

struct {
  VkPhysicalDevice device{VK_NULL_HANDLE};
  queue_family_description queue_desc;
} __context;

uint32_t device_rank(VkPhysicalDevice device) {
  VkPhysicalDeviceProperties properties;
  VkPhysicalDeviceFeatures features;
  vkGetPhysicalDeviceProperties(device, &properties);
  vkGetPhysicalDeviceFeatures(device, &features);

  if (!features.geometryShader) {
    return 0;
  }

  uint32_t rank =
      properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU ? 1000 : 0;

  if (features.tessellationShader) {
    rank += 100;
  }

  rank += properties.limits.maxImageDimension2D;
  return rank;
}

bool device_check_extensions(VkPhysicalDevice device) {
  uint32_t extension_count;
  vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count,
                                       nullptr);

  fixed_array<VkExtensionProperties> supported_extensions(extension_count);
  vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count,
                                       supported_extensions.data());

  size_t found_extensions = 0;
  for (uint32_t i = 0; i < device_extension_size; i++) {
    for (uint32_t j = 0; j < extension_count; j++) {
      if (strcmp(supported_extensions[j].extensionName, device_extensions[i]) ==
          0) {
        found_extensions++;
        break;
      }
    }
    if (found_extensions == device_extension_size)
      break;
  }

  return found_extensions == device_extension_size;
}

bool device_queue_family_valid(VkPhysicalDevice device,
                               const device_description &) {
  uint32_t q_famity_count = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &q_famity_count, nullptr);

  fixed_array<VkQueueFamilyProperties> q_families(q_famity_count);
  vkGetPhysicalDeviceQueueFamilyProperties(device, &q_famity_count,
                                           q_families.data());

  bool present = false;
  bool graphics = false;
  for (uint32_t i = 0; i < q_famity_count; i++) {

    VkBool32 supported = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(device, i, vk_window_surface_get(),
                                         &supported);

    if (supported && q_families[i].queueCount > 0) {
      present = true;
    }

    if (q_families[i].queueCount > 0 &&
        q_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      graphics = true;
    }
    if (present && graphics) {
      break;
    }
  }
  return present && graphics;
}

queue_family_description device_find_queue_family(VkPhysicalDevice device,
                                                  const device_description &) {

  queue_family_description indexes{};

  uint32_t q_famity_count = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &q_famity_count, nullptr);

  fixed_array<VkQueueFamilyProperties> q_families(q_famity_count);
  vkGetPhysicalDeviceQueueFamilyProperties(device, &q_famity_count,
                                           q_families.data());

  bool present = false;
  bool graphics = false;
  for (uint32_t i = 0; i < q_famity_count; i++) {

    VkBool32 supported = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(device, i, vk_window_surface_get(),
                                         &supported);

    if (supported && q_families[i].queueCount > 0) {
      present = true;
      indexes.present_queue_family = i;
    }

    if (q_families[i].queueCount > 0 &&
        q_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      graphics = true;
      indexes.graphics_queue_family = i;
    }
    if (present && graphics) {
      break;
    }
  }
  return indexes;
}

bool device_swap_chain_valid(VkPhysicalDevice device) {

  VkSurfaceKHR surface = vk_window_surface_get();
  uint32_t format_count = 0;
  vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count, nullptr);
  uint32_t mode_count = 0;
  vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &mode_count,
                                            nullptr);

  return format_count > 0 && mode_count > 0;
}

swap_chain_description device_swap_chain_get(VkPhysicalDevice device) {

  swap_chain_description res;

  VkSurfaceKHR surface = vk_window_surface_get();
  {

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface,
                                              &res.capabilities);
    VkExtent2D ext = res.capabilities.currentExtent;
    if (ext.width == UINT32_MAX) {
      window_size screen_size = vk_window_size();
      ext.width =
          max(res.capabilities.maxImageExtent.width,
              min(res.capabilities.minImageExtent.width, screen_size.width));
      ext.height =
          max(res.capabilities.maxImageExtent.height,
              min(res.capabilities.minImageExtent.height, screen_size.height));
    }
    res.extent = ext;
  }
  {
    uint32_t format_count = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count,
                                         nullptr);

    fixed_array<VkSurfaceFormatKHR> formats(format_count);
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count,
                                         formats.data());
    for (uint32_t i = 0; i < format_count; i++) {
      if (formats[i].format == VK_FORMAT_B8G8R8A8_UNORM &&
          formats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
        res.format = formats[i];
        break;
      }
      if ((i + 1) == format_count) {
        res.format = VkSurfaceFormatKHR{VK_FORMAT_B8G8R8A8_UNORM,
                                        VK_COLOR_SPACE_SRGB_NONLINEAR_KHR};
      }
    }
  }
  {
    uint32_t mode_count = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &mode_count,
                                              nullptr);
    fixed_array<VkPresentModeKHR> modes(mode_count);
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &mode_count,
                                              modes.data());
    for (uint32_t i = 0; i < mode_count; i++) {
      if (modes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
        res.mode = modes[i];
        break;
      }
      if ((i + 1) == mode_count) {
        res.mode = VK_PRESENT_MODE_FIFO_KHR;
      }
    }
  }
  return res;
}

bool device_supported(VkPhysicalDevice device, const device_description &desc) {
  bool valid = device_queue_family_valid(device, desc);
  bool ext_supp = device_check_extensions(device);
  bool swap_supp = ext_supp && device_swap_chain_valid(device);
  return valid && ext_supp && swap_supp;
}

bool vk_ph_device_init(const device_description &desc) {

  VkInstance instance = vk_instance_get();
  if (instance == VK_NULL_HANDLE)
    return false;

  uint32_t ph_device_count = 0;
  vkEnumeratePhysicalDevices(instance, &ph_device_count, nullptr);
  if (ph_device_count == 0)
    return false;

  fixed_array<VkPhysicalDevice> ph_device_array(ph_device_count);
  vkEnumeratePhysicalDevices(instance, &ph_device_count,
                             ph_device_array.data());

  VkPhysicalDevice ph_device{VK_NULL_HANDLE};
  {
    int _count = static_cast<int>(ph_device_count);
    uint32_t max_rank = 0;
    VkPhysicalDevice ph_tmp_device{VK_NULL_HANDLE};
    for (int i = 0; i < _count; i++) {
      VkPhysicalDevice tmp = ph_device_array[i];
      if (device_supported(tmp, desc)) {
        uint32_t rank = device_rank(tmp);
        if (rank > max_rank || ph_tmp_device == VK_NULL_HANDLE) {
          max_rank = rank;
          ph_tmp_device = ph_device_array[i];
        }
      }
    }
    ph_device = ph_tmp_device;
  }

  if (ph_device == VK_NULL_HANDLE)
    return false;
  else {
    __context.device = ph_device;
    __context.queue_desc = device_find_queue_family(ph_device, desc);
  }

  return true;
}

void vk_ph_device_deinit() { __context.device = VK_NULL_HANDLE; }

VkPhysicalDevice vk_ph_device_get() { return __context.device; }

swap_chain_description vk_ph_device_swap_chain_desc_get() {
  return device_swap_chain_get(__context.device);
}

queue_family_description vk_ph_device_queue_family_desc_get() {
  return __context.queue_desc;
}
