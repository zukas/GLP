#include "global_stack_memory.h"
#include "global_heap_memory.h"

#include "utils/math.h"
#include <cassert>
#include <mutex>

struct {
  void *buffer;
  size_t size;
  block_node_t *root;
  std::mutex lock;
} __global_stack;

#ifdef ALLOCATOR_STATS
#include <cstdio>
struct {
  uint64_t alloc_count;
  uint64_t free_count;
  size_t used_bytes;
  size_t free_bytes;
  size_t max_used;
} allocator_stats;
#endif

#ifdef ALLOCATOR_DEBUG
struct {
  locator_info *next;
} allocator_debug;
#endif

#undef global_stack_alloc

#define GLOBAL_STACK_ALIGNMENT 16

bool global_stack_init(size_t size) {
  size = align_block<GLOBAL_STACK_ALIGNMENT>(size);

  std::lock_guard<std::mutex> _lock(__global_stack.lock);
  assert(__global_stack.buffer == nullptr);

  __global_stack.buffer = global_heap_alloc(size);
  __global_stack.size = global_stack_align_size(size);
  __global_stack.root = static_cast<block_node_t *>(__global_stack.buffer);
  __global_stack.root->next = nullptr;
  __global_stack.root->size = __global_stack.size;

#ifdef ALLOCATOR_STATS
  allocator_stats.alloc_count = 0;
  allocator_stats.free_count = 0;
  allocator_stats.used_bytes = 0;
  allocator_stats.free_bytes = __global_stack.size;
  allocator_stats.max_used = 0;
#endif

  return __global_stack.buffer != nullptr;
}

void global_stack_deinit() {

  std::lock_guard<std::mutex> _lock(__global_stack.lock);
  ::global_heap_free(__global_stack.buffer, __global_stack.size);
  __global_stack.buffer = nullptr;
  __global_stack.size = 0;
  __global_stack.root = nullptr;

#ifdef ALLOCATOR_DEBUG

  locator_info *info = allocator_debug.next;
  while (info != nullptr) {
    printf("global_stack leaking pointer %p of size %lu, allocated at: %s:%u - "
           "%s\n",
           info->ptr, info->size, info->function, info->line, info->filename);
    info = info->next;
  }

#endif

#ifdef ALLOCATOR_STATS
  printf("global_stack: allocations: %lu, deallocations: %lu, used_bytes: %lu, "
         "free_bytes: %lu, "
         "max_used: %lu\n",
         allocator_stats.alloc_count, allocator_stats.free_count,
         allocator_stats.used_bytes, allocator_stats.free_bytes,
         allocator_stats.max_used);
#endif
}

size_t global_stack_align_size(size_t size) {
  return align_block<GLOBAL_STACK_ALIGNMENT>(size);
}

void *global_stack_alloc(size_t size) {
  size = align_block<GLOBAL_STACK_ALIGNMENT>(size);
  void *res = nullptr;

  {
    std::lock_guard<std::mutex> _lock(__global_stack.lock);

    block_node_t *parent = nullptr;
    block_node_t *node = __global_stack.root;
    for (;;) {
      if (node == nullptr || node->size >= size) {
        break;
      }
      parent = node;
      node = node->next;
    }
    assert(node);
    size_t rem = node->size - size;
    assert(rem >= 0);
    block_node_t *next = nullptr;
    if (rem == 0) {
      next = node->next;
    } else {
      next = address_add(node, size);
      next->next = node->next;
      next->size = rem;
    }
    if (parent) {
      parent->next = next;
    } else {
      __global_stack.root = next;
    }

    assert(__global_stack.root != nullptr);
    res = static_cast<void *>(node);

#ifdef ALLOCATOR_STATS
    allocator_stats.alloc_count++;
    allocator_stats.used_bytes += size;
    allocator_stats.free_bytes -= size;
    allocator_stats.max_used =
        max(allocator_stats.max_used, allocator_stats.used_bytes);
#endif
  }
  return res;
}

#ifdef ALLOCATOR_DEBUG

void *debug_global_stack_alloc(size_t size, const char *filename,
                               const char *function, uint32_t line) {

  size_t padded_size =
      align_block<GLOBAL_STACK_ALIGNMENT>(size + sizeof(locator_info));
  void *ptr = global_stack_alloc(padded_size);
  locator_info *info = static_cast<locator_info *>(ptr);
  info->filename = filename;
  info->function = function;
  info->line = line;
  info->size = size;
  info->ptr = address_add(ptr, sizeof(locator_info));
  info->next = allocator_debug.next;
  allocator_debug.next = info;
  return info->ptr;
}

#endif

void global_stack_free(void *ptr, size_t size) {

  assert(global_stack_owns(ptr));

#ifdef ALLOCATOR_DEBUG
  {
    std::lock_guard<std::mutex> _lock(__global_stack.lock);
    locator_info *parent = nullptr;
    locator_info *info = allocator_debug.next;
    while (info != nullptr) {
      if (info->ptr == ptr) {
        assert(
            (info->size == size || align_block<GLOBAL_STACK_ALIGNMENT>(size)) &&
            "global_stack_free - Invalid size");
        if (parent != nullptr) {
          parent->next = info->next;
        } else {
          allocator_debug.next = info->next;
        }

        ptr = info;
        break;
      }
      parent = info;
      info = info->next;
    }
  }
  size = align_block<GLOBAL_STACK_ALIGNMENT>(size + sizeof(locator_info));
#else
  size = align_block<GLOBAL_STACK_ALIGNMENT>(size);
#endif

  std::lock_guard<std::mutex> _lock(__global_stack.lock);

  block_node_t *node = __global_stack.root;

  if (ptr < node || node == nullptr) {
    block_node_t *tmp = static_cast<block_node_t *>(ptr);
    block_node_t *next = nullptr;
    size_t block_size = 0;
    void *adjusted = address_add(ptr, size);
    if (adjusted == node) {
      next = node->next;
      block_size = size + node->size;
    } else {
      next = node;
      block_size = size;
    }
    tmp->next = next;
    tmp->size = block_size;
    __global_stack.root = tmp;
  } else {

    block_node_t *next = node->next;
    while (next < ptr) {
      node = next;
      next = next->next;
    }
    size_t block_size = node->size;

    void *end_node = address_add(node, block_size);
    void *end_blk = address_add(ptr, size);

    if (end_node == ptr && end_blk == next) {
      block_size += size + node->next->size;
      next = next->next;
    } else if (end_node == ptr && end_blk < next) {
      block_size += size;
    } else if (end_node < ptr && end_blk == next) {
      block_node_t *tmp = static_cast<block_node_t *>(ptr);
      tmp->next = next->next;
      tmp->size = size + next->size;
      next = tmp;
    } else {
      block_node_t *tmp = static_cast<block_node_t *>(ptr);
      tmp->next = next;
      tmp->size = size;
      next = tmp;
    }

    node->next = next;
    node->size = block_size;
  }

#ifdef ALLOCATOR_STATS
  allocator_stats.free_count++;
  allocator_stats.used_bytes -= size;
  allocator_stats.free_bytes += size;
#endif
}

bool global_stack_owns(void *ptr) {
  return __global_stack.buffer <= ptr &&
         address_add(__global_stack.buffer, __global_stack.size) > ptr;
}
