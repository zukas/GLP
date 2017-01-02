#include "pool_allocator.h"
#include "global_heap_memory.h"

#include <cassert>

bool pool_allocator::init(size_t blk_size, size_t blk_count) {
  size_t blk = align_block<sizeof(pool_node_t)>(blk_size);
  size_t size = blk * blk_count;
  void *buffer = global_heap_alloc(size);
  if (buffer == nullptr)
    return false;
  pool_node_t *node = static_cast<pool_node_t *>(buffer);
  pool_node_t *root = node;
  for (size_t i = 1; i < blk_count - 1; i++) {
    pool_node_t *next = node + 1;
    next->next = nullptr;
    node->next = next;
    node = next;
  }
  m_root = root;
  m_buffer = buffer;
  m_size = size;
  m_blk = blk;
  return true;
}

void pool_allocator::deinit() {
  assert(m_buffer);
  global_heap_free(m_buffer, m_size);
  m_root = nullptr;
  m_buffer = nullptr;
  m_size = 0;
  m_blk = 0;
}

void *pool_allocator::alloc(size_t) {
  pool_node_t *tmp = m_root;
  m_root = m_root->next;
  return tmp;
}

void pool_allocator::free(void *p, size_t) {
  pool_node_t *ptr = static_cast<pool_node_t *>(p);
  ptr->next = m_root;
  m_root = ptr;
}
