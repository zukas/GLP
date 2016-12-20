#ifndef POOL_ALLOCATOR_H
#define POOL_ALLOCATOR_H

#include "memory.h"

template <size_t _S, size_t _On, typename __Ma = global_heap_allocator>
class pool_allocator {
public:
  pool_allocator() : m_blk(__Ma::acquire(size())) {
    pool_node_t *node = static_cast<pool_node_t *>(m_blk.ptr);
    pool_node_t *root = node;
    for (size_t i = 1; i < _On - 1; i++) {
      pool_node_t *next = node + 1;
      next->next = nullptr;
      node->next = next;
      node = next;
    }

    m_root = root;
  }

  ~pool_allocator() { __Ma::release(m_blk); }
  pool_allocator(const pool_allocator &) = delete;
  pool_allocator(pool_allocator &&) = delete;

  raw *alloc() {
    pool_node_t *tmp = m_root;
    m_root = m_root->next;
    return tmp;
  }

  void free(raw *p, size_t = 0) {
    pool_node_t *ptr = static_cast<pool_node_t *>(p);
    ptr->next = m_root;
    m_root = ptr;
  }

  size_t size() { return align_size() * _On; }
  size_t align_size(size_t = 0) {
    return align_block < sizeof(pool_node_t) < (_S);
  }

private:
  struct pool_node_t {
    pool_node_t *next;
  };

private:
  memblk m_blk;
  pool_node_t *m_root;
};

#endif // POOL_ALLOCATOR_H
