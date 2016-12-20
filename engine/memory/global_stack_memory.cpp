#include "global_stack_memory.h"
#include "global_heap_memory.h"

#include <cassert>
#include <mutex>

struct {
  raw *buffer;
  size_t size;
  block_node_t *root;
  std::mutex lock;
} __global_stack;

#ifdef ALLOCATOR_STATS
#include <cstdio>
struct {
  uint64_t count;
  size_t used_bytes;
  size_t free_bytes;
  size_t max_used;
} allocator_stats;
#endif

#define GLOBAL_STACK_ALIGNMENT 32

bool global_stack_init(size_t size) {
  size = align_block<GLOBAL_STACK_ALIGNMENT>(size);

  std::lock_guard<std::mutex> _lock(__global_stack.lock);
  assert(m_buffer == nullptr);

  __global_stack.buffer = global_heap_alloc(size);
  __global_stack.size = global_stack_align_size(size);
  __global_stack.root = static_cast<block_node_t *>(__global_stack.buffer);
  __global_stack.root->next = nullptr;
  __global_stack.root->size = __global_stack.size;

#ifdef ALLOCATOR_STATS
  allocator_stats.count = 0;
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

#ifdef ALLOCATOR_STATS
  printf("global_stack: allocations: %lu, used_bytes: %lu, free_bytes: %lu, "
         "max_used: %lu\n",
         allocator_stats.count, allocator_stats.used_bytes,
         allocator_stats.free_bytes, allocator_stats.max_used);
#endif
}

size_t global_stack_align_size(size_t size) {
  return align_block<GLOBAL_STACK_ALIGNMENT>(size);
}

raw *global_stack_alloc(size_t size) {
  size = align_block<GLOBAL_STACK_ALIGNMENT>(size);
  raw *res = nullptr;

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

    assert(m_root != nullptr);
    res = static_cast<raw *>(node);

#ifdef ALLOCATOR_STATS
    allocator_stats.count++;
    allocator_stats.used_bytes += size;
    allocator_stats.free_bytes -= size;
    allocator_stats.max_used =
        max(allocator_stats.max_used, allocator_stats.used_bytes);
#endif
  }
  return res;
}

void global_stack_free(raw *ptr, size_t size) {

  assert(global_stack_owns(ptr));
  size = align_block<GLOBAL_STACK_ALIGNMENT>(size);

  std::lock_guard<std::mutex> _lock(__global_stack.lock);

  block_node_t *node = __global_stack.root;

  if (ptr < node || node == nullptr) {
    block_node_t *tmp = static_cast<block_node_t *>(ptr);
    block_node_t *next = nullptr;
    size_t block_size = 0;
    raw *adjusted = address_add(ptr, size);
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

    raw *end_node = address_add(node, block_size);
    raw *end_blk = address_add(ptr, size);

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
  allocator_stats.used_bytes -= size;
  allocator_stats.free_bytes += size;
#endif
}

bool global_stack_owns(raw *ptr) {
  return __global_stack.buffer >= ptr &&
         address_add(__global_stack.buffer, __global_stack.size) < ptr;
}
