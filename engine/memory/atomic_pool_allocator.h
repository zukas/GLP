#ifndef ATOMIC_POOL_ALLOCATOR_H
#define ATOMIC_POOL_ALLOCATOR_H

#include "global_heap_memory.h"

#include <atomic>

template <size_t _S, size_t _On, typename __Ma = global_heap_allocator>
class atomic_pool_allocator : __Ma {
public:
  atomic_pool_allocator() : m_buffer(__Ma::alloc(size())) {
    atomic_pool_node_t *node = static_cast<atomic_pool_node_t *>(m_buffer);
    atomic_pool_node_t *root = node;
    for (size_t i = 1; i < _On - 1; i++) {
      atomic_pool_node_t *next = node + 1;
      next->next = nullptr;
      node->next = next;
      node = next;
    }

    m_root = root;
  }

  ~atomic_pool_allocator() { __Ma::free(m_buffer, size()); }
  atomic_pool_allocator(const atomic_pool_allocator &) = delete;
  atomic_pool_allocator(atomic_pool_allocator &&) = delete;

  raw *alloc() {
    atomic_pool_node_t *tmp = m_root.load(std::memory_order_acquire);
    while (!m_root.compare_exchange_strong(tmp, tmp->next,
                                           std::memory_order_acq_rel)) {
    }

    return tmp;
  }

  void free(raw *p, size_t = 0) {
    atomic_pool_node_t *ptr = static_cast<atomic_pool_node_t *>(p);
    atomic_pool_node_t *tmp = m_root.load(std::memory_order_acquire);
    ptr->next.store(tmp, std::memory_order_release);
    while (
        !m_root.compare_exchange_strong(tmp, ptr, std::memory_order_acq_rel)) {
      ptr->next.store(tmp, std::memory_order_release);
    }
  }

  constexpr size_t size() const { return align_size() * _On; }
  size_t align_size(size_t = 0) const {
    return align_block<sizeof(atomic_pool_node_t)>(_S);
  }

private:
  struct atomic_pool_node_t {
    std::atomic<atomic_pool_node_t *> next;
  };

private:
  raw *m_buffer;
  std::atomic<atomic_pool_node_t *> m_root;
};

#endif // ATOMIC_POOL_ALLOCATOR_H
