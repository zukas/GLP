#ifndef ATOMIC_POOL_ALLOCATOR_H
#define ATOMIC_POOL_ALLOCATOR_H

#include "memory.h"

#include <atomic>

template <typename _Tp, size_t _On, typename __Ma = sys_allocator>
class atomic_pool_allocator {

public:
  atomic_pool_allocator() : m_blk(__Ma::acquire(size())) {
    atomic_pool_node_t *node = static_cast<atomic_pool_node_t *>(m_blk.ptr);
    atomic_pool_node_t *root = node;
    for (size_t i = 1; i < _On - 1; i++) {
      atomic_pool_node_t *next = node + 1;
      next->next = nullptr;
      node->next = next;
      node = next;
    }

    m_root = root;
  }

  ~atomic_pool_allocator() { __Ma::release(m_blk); }
  atomic_pool_allocator(const atomic_pool_allocator &) = delete;
  atomic_pool_allocator(atomic_pool_allocator &&) = delete;

  _Tp *alloc() {
    atomic_pool_node_t *tmp = m_root.load(std::memory_order_acquire);
    while (!m_root.compare_exchange_strong(tmp, tmp->next,
                                           std::memory_order_acq_rel)) {
    }

    return static_cast<_Tp *>(static_cast<raw *>(tmp));
  }

  void free(_Tp *p) {
    atomic_pool_node_t *ptr =
        static_cast<atomic_pool_node_t *>(static_cast<raw *>(p));
    atomic_pool_node_t *tmp = m_root.load(std::memory_order_acquire);
    ptr->next.store(tmp, std::memory_order_release);
    while (
        !m_root.compare_exchange_strong(tmp, ptr, std::memory_order_acq_rel)) {
      ptr->next.store(tmp, std::memory_order_release);
    }
  }

  static size_t size() { return sizeof(_Tp) * _On; }

private:
  struct atomic_pool_node_t {
    std::atomic<atomic_pool_node_t *> next;
  };

private:
  memblk m_blk;
  std::atomic<atomic_pool_node_t *> m_root;
};

#endif // ATOMIC_POOL_ALLOCATOR_H
