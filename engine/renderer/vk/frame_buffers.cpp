#include "frame_buffers.h"

#include "image_views.h"
#include "internal/types.h"
#include "logical_device.h"
#include "memory/global_malloc.h"
#include "memory/mutils.h"
#include "memory/pool_allocator.h"
#include "memory/type_allocator.h"
#include "swap_chain.h"

using allocator = type_allocator<VkFramebuffersExt_T, pool_allocator>;

struct {
  allocator frame_buffer_allocator;
} __context;

bool vk_frame_buffers_init(size_t count) {
  return __context.frame_buffer_allocator.init(sizeof(VkFramebuffersExt_T),
                                               count);
}

void vk_frame_buffers_deinit() { __context.frame_buffer_allocator.deinit(); }

VkFramebuffersExt
vk_frame_buffers_create(const pipline_description &pipeline_desc,
                        const VkPipelineExt pipeline) {

  size_t image_v_size = vk_image_views_size();
  VkImageView *image_v = vk_image_views_get();

  VkExtent2D extent = vk_swap_chain_extent_get();
  VkDevice device = vk_lg_device_get();

  VkFramebuffer *buffers = static_cast<VkFramebuffer *>(
      glp_malloc(array_size_bytes<VkFramebuffer>(image_v_size)));

  uint32_t i;
  for (i = 0; i < image_v_size; i++) {
    VkFramebufferCreateInfo buffer_info = {};
    buffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    buffer_info.renderPass = pipeline->render_pass;
    buffer_info.attachmentCount = 1;
    buffer_info.pAttachments = &image_v[i];
    buffer_info.width = extent.width;
    buffer_info.height = extent.height;
    buffer_info.layers = 1;
    if (vkCreateFramebuffer(device, &buffer_info, nullptr, &buffers[i]) !=
        VK_SUCCESS)
      break;
  }
  VkFramebuffersExt_T *obj = nullptr;
  if (i == image_v_size) {
    obj = __context.frame_buffer_allocator.make_new();
    obj->frame_buffers = buffers;
    obj->size = image_v_size;
  } else {
    for (uint32_t j = 0; j < i; j++) {
      vkDestroyFramebuffer(device, buffers[i], nullptr);
    }
    glp_free(buffers);
  }
  return obj;
}

void vk_frame_buffers_destroy(const VkFramebuffersExt frame_buffers) {
  if (frame_buffers) {
    VkFramebuffer *buffers = frame_buffers->frame_buffers;
    size_t buffers_size = frame_buffers->size;
    if (buffers) {
      VkDevice device = vk_lg_device_get();
      for (uint32_t i = 0; i < buffers_size; i++) {
        vkDestroyFramebuffer(device, buffers[i], nullptr);
      }
      glp_free(buffers);
    }
    __context.frame_buffer_allocator.destroy(frame_buffers);
  }
}
