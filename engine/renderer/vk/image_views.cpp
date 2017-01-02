#include "image_views.h"

#include "logical_device.h"
#include "swap_chain.h"
#include "utils/fixed_array.h"

struct {
  fixed_array<VkImageView> image_views;
} __context;

bool vk_image_views_init() {

  VkDevice device = vk_lg_device_get();
  VkSwapchainKHR swap_chain = vk_swap_chain_get();
  uint32_t image_count = 0;
  vkGetSwapchainImagesKHR(device, swap_chain, &image_count, nullptr);
  fixed_array<VkImage> swap_images(image_count);
  vkGetSwapchainImagesKHR(device, swap_chain, &image_count, swap_images.data());

  __context.image_views = fixed_array<VkImageView>(image_count);

  bool status = true;
  for (uint32_t i = 0; i < image_count; i++) {

    VkImageViewCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    create_info.format = vk_swap_chain_image_format_get();
    create_info.image = swap_images[i];
    create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    create_info.subresourceRange.baseMipLevel = 0;
    create_info.subresourceRange.levelCount = 1;
    create_info.subresourceRange.baseArrayLayer = 0;
    create_info.subresourceRange.layerCount = 1;

    status = vkCreateImageView(device, &create_info, nullptr,
                               &__context.image_views[i]) == VK_SUCCESS;
  }
  return status;
}

void vk_image_views_deinit() {

  VkDevice device = vk_lg_device_get();
  for (uint32_t i = 0, l = __context.image_views.size<uint32_t>(); i < l; i++) {
    vkDestroyImageView(device, __context.image_views[i], nullptr);
  }
  __context.image_views.reset();
}

VkImageView *vk_image_views_get() { return __context.image_views.data(); }

size_t vk_image_views_size() { return __context.image_views.size(); }
