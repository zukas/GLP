#include "command_buffers.h"

#include "internal/types.h"
#include "logical_device.h"
#include "memory/global_malloc.h"
#include "memory/pool_allocator.h"
#include "memory/type_allocator.h"
#include "phisical_device.h"
#include "swap_chain.h"

#include <cassert>

using allocator = type_allocator<VkCommandBuffersExt_T, pool_allocator>;

struct {
  VkCommandPool command_pool;
  allocator command_allocator;
} __context;

bool vk_cmd_pool_init(size_t count) {

  bool status =
      __context.command_allocator.init(sizeof(VkCommandBuffersExt_T), count);
  if (status) {
    queue_family_description queue_desc = vk_ph_device_queue_family_desc_get();
    VkCommandPoolCreateInfo pool_info{};
    pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    pool_info.queueFamilyIndex = queue_desc.graphics_queue_family;
    pool_info.flags = 0; // Optional

    status = vkCreateCommandPool(vk_lg_device_get(), &pool_info, nullptr,
                                 &__context.command_pool) == VK_SUCCESS;
  }
  return status;
}

void vk_cmd_pool_deinit() {
  vkDestroyCommandPool(vk_lg_device_get(), __context.command_pool, nullptr);
  __context.command_allocator.deinit();
}

VkCommandBuffer vk_cmd_create() {
  VkCommandBufferAllocateInfo alloc_info = {};
  alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  alloc_info.commandPool = __context.command_pool;
  alloc_info.commandBufferCount = 1;

  VkCommandBuffer buffer;
  vkAllocateCommandBuffers(vk_lg_device_get(), &alloc_info, &buffer);
  return buffer;
}

void vk_cmd_destroy(VkCommandBuffer buffer) {
  vkFreeCommandBuffers(vk_lg_device_get(), __context.command_pool, 1, &buffer);
}

VkCommandBuffersExt
vk_cmd_buffers_create(const VkFramebuffersExt frame_buffers) {

  VkCommandBufferAllocateInfo alloc_info{};
  alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  alloc_info.commandPool = __context.command_pool;
  alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  alloc_info.commandBufferCount = frame_buffers->size;

  VkCommandBuffer *buffers = static_cast<VkCommandBuffer *>(
      glp_malloc(array_size_bytes<VkCommandBuffer>(frame_buffers->size)));

  VkResult res =
      vkAllocateCommandBuffers(vk_lg_device_get(), &alloc_info, buffers);
  if (res != VK_SUCCESS) {
    glp_free(buffers);
    return nullptr;
  }

  VkCommandBuffersExt_T *obj = __context.command_allocator.make_new();
  obj->command_buffers = buffers;
  obj->size = frame_buffers->size;
  return obj;
}

void vk_cmd_buffers_destroy(const VkCommandBuffersExt cmd_buffers) {
  assert(cmd_buffers);
  vkFreeCommandBuffers(vk_lg_device_get(), __context.command_pool,
                       cmd_buffers->size, cmd_buffers->command_buffers);

  glp_free(cmd_buffers->command_buffers);
  __context.command_allocator.destroy(cmd_buffers);
}

void vk_cmd_buffers_record(const VkPipelineExt pipeline,
                           const VkFramebuffersExt frame_buffers,
                           const VkCommandBuffersExt cmd_buffers,
                           geometry_description geometry) {

  VkExtent2D extent = vk_swap_chain_extent_get();

  VkViewport viewport = {};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = static_cast<float>(extent.width);
  viewport.height = static_cast<float>(extent.height);
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;

  VkRect2D scissor = {};
  scissor.offset = {0, 0};
  scissor.extent = extent;

  for (size_t i = 0; i < cmd_buffers->size; i++) {
    VkCommandBufferBeginInfo record_info = {};
    record_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    record_info.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
    record_info.pInheritanceInfo = nullptr; // Optional

    vkBeginCommandBuffer(cmd_buffers->command_buffers[i], &record_info);

    VkRenderPassBeginInfo render_info = {};
    render_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    render_info.renderPass = pipeline->render_pass;
    render_info.framebuffer = frame_buffers->frame_buffers[i];
    render_info.renderArea.offset = {0, 0};
    render_info.renderArea.extent = extent;

    VkClearValue clear_value;
    clear_value.color = {{0.2f, 0.4f, 0.3f, 1.f}};
    render_info.clearValueCount = 1;
    render_info.pClearValues = &clear_value;

    vkCmdBeginRenderPass(cmd_buffers->command_buffers[i], &render_info,
                         VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(cmd_buffers->command_buffers[i],
                      VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->pipeline);

    vkCmdSetViewport(cmd_buffers->command_buffers[i], 0, 1, &viewport);
    vkCmdSetScissor(cmd_buffers->command_buffers[i], 0, 1, &scissor);

    vkCmdBindVertexBuffers(cmd_buffers->command_buffers[i], 0, 1,
                           &geometry.vertex_data->buffer,
                           &geometry.vertex_data->block.offset);

    if (geometry.index_count > 0) {
      vkCmdBindIndexBuffer(
          cmd_buffers->command_buffers[i], geometry.index_data->buffer,
          geometry.index_data->block.offset, VK_INDEX_TYPE_UINT16);
      vkCmdDrawIndexed(cmd_buffers->command_buffers[i], geometry.index_count, 1,
                       0, 0, 0);
    } else {

      vkCmdDraw(cmd_buffers->command_buffers[i], geometry.vertex_count, 1, 0,
                0);
    }

    vkCmdEndRenderPass(cmd_buffers->command_buffers[i]);

    vkEndCommandBuffer(cmd_buffers->command_buffers[i]);
  }
}
