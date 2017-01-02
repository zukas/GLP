#include "swap_chain.h"

#include "logical_device.h"
#include "phisical_device.h"
#include "utils/fixed_array.h"
#include "window.h"

struct {
  VkSwapchainKHR swap_chain{VK_NULL_HANDLE};
  VkFormat swap_chain_format;
  VkExtent2D swap_chain_extent;
} __context;

bool vk_swap_chain_init() {
  swap_chain_description swap = vk_ph_device_swap_chain_desc_get();
  uint32_t image_count = swap.capabilities.minImageCount + 1;
  if (swap.capabilities.maxImageCount > 0 &&
      image_count > swap.capabilities.maxImageCount) {
    image_count = swap.capabilities.maxImageCount;
  }

  VkSwapchainCreateInfoKHR createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  createInfo.surface = vk_window_surface_get();
  createInfo.minImageCount = image_count;
  createInfo.imageFormat = swap.format.format;
  createInfo.imageColorSpace = swap.format.colorSpace;
  createInfo.imageExtent = swap.extent;
  createInfo.imageArrayLayers = 1;
  createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

  queue_family_description queue_desc = vk_ph_device_queue_family_desc_get();
  uint32_t indexes[] = {queue_desc.graphics_queue_family,
                        queue_desc.present_queue_family};

  if (queue_desc.graphics_queue_family != queue_desc.present_queue_family) {
    createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    createInfo.queueFamilyIndexCount = 2;
    createInfo.pQueueFamilyIndices = indexes;
  } else {
    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.queueFamilyIndexCount = 0;     // Optional
    createInfo.pQueueFamilyIndices = nullptr; // Optional
  }

  createInfo.preTransform = swap.capabilities.currentTransform;
  createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  createInfo.presentMode = swap.mode;
  createInfo.clipped = VK_TRUE;
  createInfo.oldSwapchain = VK_NULL_HANDLE;

  VkResult res = vkCreateSwapchainKHR(vk_lg_device_get(), &createInfo, nullptr,
                                      &__context.swap_chain);
  if (res == VK_SUCCESS) {
    __context.swap_chain_format = swap.format.format;
    __context.swap_chain_extent = swap.extent;
  }

  return res == VK_SUCCESS;
}

void vk_swap_chain_deinit() {
  vkDestroySwapchainKHR(vk_lg_device_get(), __context.swap_chain, nullptr);
  __context.swap_chain = nullptr;
}

VkSwapchainKHR vk_swap_chain_get() { return __context.swap_chain; }

VkFormat vk_swap_chain_image_format_get() {
  return __context.swap_chain_format;
}

VkExtent2D vk_swap_chain_extent_get() {}
