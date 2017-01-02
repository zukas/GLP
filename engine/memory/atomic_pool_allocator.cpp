#include "atomic_pool_allocator.h"
#include "global_heap_memory.h"

bool atomic_pool_allocator::init(size_t blk_size, size_t blk_count) {
  size_t blk = align_block<sizeof(atomic_pool_node_t)>(blk_size);
  size_t size = blk * blk_count;
  void *buffer = global_heap_alloc(size);
  if (buffer == nullptr)
    return false;

  atomic_pool_node_t *node = static_cast<atomic_pool_node_t *>(buffer);
  atomic_pool_node_t *root = node;
  for (size_t i = 1; i < blk_count - 1; i++) {
    atomic_pool_node_t *next = node + 1;
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

void atomic_pool_allocator::deinit() { global_heap_free(m_buffer, m_size); }

void *atomic_pool_allocator::alloc(size_t) {
  atomic_pool_node_t *tmp = m_root.load(std::memory_order_acquire);
  while (!m_root.compare_exchange_strong(tmp, tmp->next,
                                         std::memory_order_acq_rel)) {
  }

  return tmp;
}

void atomic_pool_allocator::free(void *p, size_t) {
  atomic_pool_node_t *ptr = static_cast<atomic_pool_node_t *>(p);
  atomic_pool_node_t *tmp = m_root.load(std::memory_order_acquire);
  ptr->next.store(tmp, std::memory_order_release);
  while (!m_root.compare_exchange_strong(tmp, ptr, std::memory_order_acq_rel)) {
    ptr->next.store(tmp, std::memory_order_release);
  }
}
