#include "global_heap_memory.h"

#include "utils/math.h"
#include <cassert>
#include <mutex>

struct {
  void *buffer;
  size_t size;
  block_node_t *root;
  std::mutex lock;
} __global_heap;

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

#define GLOBAL_HEAP_ALIGNMENT MB(1)

bool global_heap_init(size_t size) {
  size = align_block<GLOBAL_HEAP_ALIGNMENT>(size);

  std::lock_guard<std::mutex> _lock(__global_heap.lock);
  assert(__global_heap.buffer == nullptr);

  __global_heap.buffer = ::malloc(size);
  __global_heap.size = size;
  __global_heap.root = static_cast<block_node_t *>(__global_heap.buffer);
  __global_heap.root->next = nullptr;
  __global_heap.root->size = size;

#ifdef ALLOCATOR_STATS
  allocator_stats.alloc_count = 0;
  allocator_stats.free_count = 0;
  allocator_stats.used_bytes = 0;
  allocator_stats.free_bytes = size;
  allocator_stats.max_used = 0;
#endif

  return __global_heap.buffer != nullptr;
}

void global_heap_deinit() {

  std::lock_guard<std::mutex> _lock(__global_heap.lock);
  ::free(__global_heap.buffer);
  __global_heap.buffer = nullptr;
  __global_heap.size = 0;
  __global_heap.root = nullptr;

#ifdef ALLOCATOR_STATS
  printf("global_heap: allocations: %lu, deallocations: %lu, used_bytes: %lu, "
         "free_bytes: %lu, "
         "max_used: %lu\n",
         allocator_stats.alloc_count, allocator_stats.free_count,
         allocator_stats.used_bytes, allocator_stats.free_bytes,
         allocator_stats.max_used);
#endif
}

size_t global_heap_align_size(size_t size) {
  return align_block<GLOBAL_HEAP_ALIGNMENT>(size);
}

void *global_heap_alloc(size_t size) {
  size = align_block<GLOBAL_HEAP_ALIGNMENT>(size);
  void *res = nullptr;

  {
    std::lock_guard<std::mutex> _lock(__global_heap.lock);

    block_node_t *parent = nullptr;
    block_node_t *node = __global_heap.root;
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
      __global_heap.root = next;
    }

    assert(__global_heap.root != nullptr);
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

void global_heap_free(void *ptr, size_t size) {

  assert(global_heap_owns(ptr));
  size = align_block<GLOBAL_HEAP_ALIGNMENT>(size);

  std::lock_guard<std::mutex> _lock(__global_heap.lock);

  block_node_t *node = __global_heap.root;

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
    __global_heap.root = tmp;
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

bool global_heap_owns(void *ptr) {
  return __global_heap.buffer <= ptr &&
         address_add(__global_heap.buffer, __global_heap.size) > ptr;
}
