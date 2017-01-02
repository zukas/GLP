#ifndef ATOMIC_POOL_ALLOCATOR_H
#define ATOMIC_POOL_ALLOCATOR_H

#include "mutils.h"
#include <atomic>

class atomic_pool_allocator {
public:
  bool init(size_t blk_size, size_t blk_count);
  void deinit();

  void *alloc(size_t = 0);
  void free(void *p, size_t = 0);

  inline size_t size() const { return m_size; }
  inline size_t align_size(size_t = 0) const { return m_blk; }

private:
  struct atomic_pool_node_t {
    std::atomic<atomic_pool_node_t *> next;
  };

private:
  std::atomic<atomic_pool_node_t *> m_root;
  void *m_buffer;
  size_t m_size;
  size_t m_blk;
};

#endif // ATOMIC_POOL_ALLOCATOR_H
