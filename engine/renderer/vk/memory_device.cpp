#include "memory_device.h"
#include "logical_device.h"
#include "memory/mutils.h"
#include "memory/pool_allocator.h"
#include "memory/type_allocator.h"
#include "phisical_device.h"
#include <cassert>

// enum VkMemoryAccess {
//  VK_LOCAL = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
//  VK_HOST_MODIFY = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
//};

struct gpu_block_node_t {
  gpu_block_node_t *next{nullptr};
  VkDeviceSize offset{0};
  VkDeviceSize size{0};
};

using allocator = type_allocator<gpu_block_node_t, pool_allocator>;

struct gpu_dynamic_block_t {
  allocator alloc;
  gpu_block_node_t *root{nullptr};
};

struct {
  VkDeviceMemory host_stage_memory{VK_NULL_HANDLE};
  VkDeviceMemory device_geom_memory{VK_NULL_HANDLE};
  gpu_dynamic_block_t stage_manager;
  gpu_dynamic_block_t geom_manager;
} __context;

VkMemoryBlockExt acquire_mem_blk(VkDeviceMemory dev_mem,
                                 gpu_dynamic_block_t &manager,
                                 VkDeviceSize size) {
  gpu_block_node_t *parent = nullptr;
  gpu_block_node_t *node = manager.root;
  for (;;) {
    if (node == nullptr || node->size >= size)
      break;
    parent = node;
    node = node->next;
  }
  assert(node);
  VkDeviceSize rem = node->size - size;
  assert(rem >= 0);
  gpu_block_node_t *next = nullptr;
  if (rem == 0) {
    next = node->next;
  } else {
    next = manager.alloc.make_new();
    next->next = nullptr;
    next->offset = node->offset + size;
    next->size = rem;
  }

  if (parent) {
    parent->next = next;
  } else {
    manager.root = next;
  }
  return VkMemoryBlockExt{dev_mem, node->offset, size, node};
}

void release_mem_blk(gpu_dynamic_block_t &manager, VkMemoryBlockExt blk) {
  gpu_block_node_t *node = manager.root;
  gpu_block_node_t *tmp = static_cast<gpu_block_node_t *>(blk.state);

  if (blk.offset < node->offset || node == nullptr) {
    gpu_block_node_t *next = nullptr;
    VkDeviceSize block_size = 0;
    if (blk.offset + blk.size == node->offset) {
      next = node->next;
      block_size = blk.size + node->size;
      manager.alloc.destroy(node);
    } else {
      next = node;
      block_size = blk.size;
    }
    tmp->next = next;
    tmp->offset = blk.offset;
    tmp->size = block_size;
    manager.root = tmp;
  } else {
    gpu_block_node_t *next = node->next;
    while (next && next->offset < blk.offset) {
      node = next;
      next = next->next;
    }
    VkDeviceSize block_size = node->size;
    VkDeviceSize end_node = node->offset + block_size;
    VkDeviceSize end_blk = blk.offset + blk.size;

    if (end_node == blk.offset && end_blk == next->offset) {
      block_size += blk.size + next->size;
      next = next->next;
      // release tmp and next
      manager.alloc.destroy(tmp);
      manager.alloc.destroy(next);
    } else if (end_node == blk.offset && end_blk < next->offset) {
      block_size += blk.size;
      // release tmp
      manager.alloc.destroy(tmp);
    } else if (end_node < blk.offset && end_blk == next->offset) {
      tmp->next = next->next;
      tmp->size = blk.size + next->size;
      tmp->offset = blk.offset;
      // release next
      manager.alloc.destroy(next);
      next = tmp;
    } else {
      tmp->next = next;
      tmp->size = blk.size;
      tmp->offset = blk.offset;
      next = tmp;
    }
    node->next = next;
    node->size = block_size;
  }
}

uint32_t memory_type_index(uint32_t filter, VkMemoryPropertyFlags flags) {
  VkPhysicalDeviceMemoryProperties props;
  vkGetPhysicalDeviceMemoryProperties(vk_ph_device_get(), &props);
  for (uint32_t i = 0; i < props.memoryTypeCount; i++) {
    if (filter & (1 << i) &&
        (props.memoryTypes[i].propertyFlags & flags) == flags)
      return i;
  }
  return UINT32_MAX;
}

VkDeviceMemory allocate_memory(VkDevice device, VkBufferUsageFlags usage,
                               VkMemoryAccess access, size_t size) {

  VkBufferCreateInfo buffer_info{};
  buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  buffer_info.size = size;
  buffer_info.usage = usage;
  buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  VkBuffer buffer;
  vkCreateBuffer(device, &buffer_info, nullptr, &buffer);

  VkMemoryRequirements req;
  vkGetBufferMemoryRequirements(device, buffer, &req);

  VkMemoryAllocateInfo allocate_info = {};
  allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocate_info.allocationSize = req.size;
  allocate_info.memoryTypeIndex = memory_type_index(req.memoryTypeBits, access);
  VkDeviceMemory memory;
  vkAllocateMemory(device, &allocate_info, nullptr, &memory);

  vkDestroyBuffer(device, buffer, nullptr);
  return memory;
}

bool vk_memory_init(size_t host_stage_memory, size_t device_geom_memory,
                    size_t) {

  host_stage_memory = align_block<64>(host_stage_memory);
  device_geom_memory = align_block<64>(device_geom_memory);

  VkDevice device = vk_lg_device_get();

  __context.host_stage_memory =
      allocate_memory(device, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_HOST_MODIFY,
                      host_stage_memory);
  __context.device_geom_memory =
      allocate_memory(device, VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                                  VK_BUFFER_USAGE_INDEX_BUFFER_BIT |
                                  VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
                                  VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                      VK_LOCAL, device_geom_memory);

  bool status = __context.host_stage_memory != VK_NULL_HANDLE &&
                __context.device_geom_memory != VK_NULL_HANDLE;
  if (status) {
    __context.stage_manager.alloc.init(sizeof(gpu_block_node_t), 4096);
    __context.stage_manager.root = __context.stage_manager.alloc.make_new();
    __context.stage_manager.root->next = nullptr;
    __context.stage_manager.root->offset = 0;
    __context.stage_manager.root->size = host_stage_memory;

    __context.geom_manager.alloc.init(sizeof(gpu_block_node_t), 4096);
    __context.geom_manager.root = __context.geom_manager.alloc.make_new();
    __context.geom_manager.root->next = nullptr;
    __context.geom_manager.root->offset = 0;
    __context.geom_manager.root->size = device_geom_memory;
  }

  return status;
}

void vk_memory_deinit() {

  VkDevice device = vk_lg_device_get();
  if (__context.host_stage_memory)
    vkFreeMemory(device, __context.host_stage_memory, nullptr);
  if (__context.device_geom_memory)
    vkFreeMemory(device, __context.device_geom_memory, nullptr);
  __context.host_stage_memory = VK_NULL_HANDLE;
  __context.device_geom_memory = VK_NULL_HANDLE;

  __context.stage_manager.alloc.deinit();
  __context.stage_manager.root = nullptr;

  __context.geom_manager.alloc.deinit();
  __context.geom_manager.root = nullptr;
}

VkMemoryBlockExt vk_memory_stage_blk_acquire(const VkBuffer buffer) {

  VkDevice device = vk_lg_device_get();
  VkMemoryRequirements req;
  vkGetBufferMemoryRequirements(device, buffer, &req);
  return acquire_mem_blk(__context.host_stage_memory, __context.stage_manager,
                         req.size);
}

VkMemoryBlockExt vk_memory_gpu_blk_acquire(const VkBuffer buffer) {
  VkDevice device = vk_lg_device_get();
  VkMemoryRequirements req;
  vkGetBufferMemoryRequirements(device, buffer, &req);
  return acquire_mem_blk(__context.device_geom_memory, __context.geom_manager,
                         req.size);
}

void vk_memory_blk_release(VkMemoryBlockExt blk) {
  if (blk.memory == __context.host_stage_memory) {
    release_mem_blk(__context.stage_manager, blk);
  } else {
    release_mem_blk(__context.geom_manager, blk);
  }
}
