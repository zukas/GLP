#include "vertex.h"
#include "commands.h"
#include "internal/types.h"
#include "logical_device.h"
#include "memory/pool_allocator.h"
#include "memory/type_allocator.h"
#include "memory_device.h"
#include "queue.h"

#include <cstring>

using allocator = type_allocator<VkVertexBufferExt_T, pool_allocator>;

struct {
  allocator vertex_allocator;
} __context;

bool vk_vertex_buffer_init(size_t size) {
  __context.vertex_allocator.init(sizeof(VkVertexBufferExt_T), size);
}

void vk_vertex_buffer_deinit() { __context.vertex_allocator.deinit(); }

VkVertexBufferExt vk_vertex_buffer_create(vertex_2d_color *data, size_t size) {

  VkDevice device = vk_lg_device_get();

  VkBuffer src_buffer;
  VkBuffer dst_buffer;
  VkMemoryBlockExt src_block;
  VkMemoryBlockExt dst_block;

  VkBufferCreateInfo buffer_info = {};
  buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  buffer_info.size = sizeof(vertex_2d_color) * size;
  buffer_info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
  buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  VkResult res = vkCreateBuffer(device, &buffer_info, nullptr, &src_buffer);
  if (res != VK_SUCCESS) {
    return nullptr;
  }
  buffer_info.usage =
      VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
  res = vkCreateBuffer(device, &buffer_info, nullptr, &dst_buffer);
  if (res != VK_SUCCESS) {
    vkDestroyBuffer(device, src_buffer, nullptr);
    return nullptr;
  }
  src_block = vk_memory_stage_blk_acquire(src_buffer);
  dst_block = vk_memory_gpu_blk_acquire(dst_buffer);

  res = vkBindBufferMemory(device, src_buffer, src_block.memory,
                           src_block.offset);
  if (res != VK_SUCCESS) {
    vkDestroyBuffer(device, src_buffer, nullptr);
    vkDestroyBuffer(device, dst_buffer, nullptr);
    vk_memory_blk_release(src_block);
    vk_memory_blk_release(dst_block);
    return nullptr;
  }

  res = vkBindBufferMemory(device, dst_buffer, dst_block.memory,
                           dst_block.offset);
  if (res != VK_SUCCESS) {
    vkDestroyBuffer(device, src_buffer, nullptr);
    vkDestroyBuffer(device, dst_buffer, nullptr);
    vk_memory_blk_release(src_block);
    vk_memory_blk_release(dst_block);
    return nullptr;
  }

  void *raw_memory = nullptr;
  res = vkMapMemory(device, src_block.memory, src_block.offset, src_block.size,
                    0, &raw_memory);
  if (res != VK_SUCCESS) {
    vkDestroyBuffer(device, src_buffer, nullptr);
    vkDestroyBuffer(device, dst_buffer, nullptr);
    vk_memory_blk_release(src_block);
    vk_memory_blk_release(dst_block);
    return nullptr;
  }

  memcpy(raw_memory, data, src_block.size);

  VkMappedMemoryRange range_info{};
  range_info.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
  range_info.memory = src_block.memory;
  range_info.offset = src_block.offset;
  range_info.size = src_block.size;
  vkFlushMappedMemoryRanges(device, 1, &range_info);

  vkUnmapMemory(device, src_block.memory);

  VkCommandBuffer cmd = vk_cmd_create();

  VkCommandBufferBeginInfo transfer_cmd_info{};
  transfer_cmd_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  transfer_cmd_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

  vkBeginCommandBuffer(cmd, &transfer_cmd_info);

  VkBufferCopy copy_info{};
  copy_info.srcOffset = src_block.offset;
  copy_info.dstOffset = dst_block.offset;
  copy_info.size = src_block.size;

  vkCmdCopyBuffer(cmd, src_buffer, dst_buffer, 1, &copy_info);

  VkBufferMemoryBarrier barrier{};
  barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
  barrier.buffer = dst_buffer;
  barrier.dstAccessMask = VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;
  barrier.srcAccessMask = VK_ACCESS_MEMORY_WRITE_BIT;
  barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.offset = dst_block.offset;
  barrier.size = dst_block.size;

  vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TRANSFER_BIT,
                       VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, 0, 0, nullptr, 1,
                       &barrier, 0, nullptr);

  vkEndCommandBuffer(cmd);

  VkSubmitInfo submit{};

  submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submit.pCommandBuffers = &cmd;
  submit.commandBufferCount = 1;

  vkQueueSubmit(vk_queue_graphics_get(), 1, &submit, VK_NULL_HANDLE);

  VkVertexBufferExt_T *obj = __context.vertex_allocator.make_new();
  obj->buffer = dst_buffer;
  obj->block = dst_block;

  vkDeviceWaitIdle(device);

  vkDestroyBuffer(device, src_buffer, nullptr);
  vk_memory_blk_release(src_block);

  return obj;
}

void vk_vertex_buffer_destroy(const VkVertexBufferExt buffer) {
  assert(buffer != nullptr);
  vk_memory_blk_release(buffer->block);
  vkDestroyBuffer(vk_lg_device_get(), buffer->buffer, nullptr);
  __context.vertex_allocator.destroy(buffer);
}
