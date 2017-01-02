#ifndef POOL_ALLOCATOR_H
#define POOL_ALLOCATOR_H

#include "mutils.h"

class pool_allocator {
public:
  bool init(size_t blk_size, size_t blk_count);
  void deinit();

  void *alloc(size_t = 0);
  void free(void *p, size_t = 0);

  inline size_t size() { return m_size; }
  inline size_t align_size(size_t = 0) { return m_blk; }

private:
  struct pool_node_t {
    pool_node_t *next;
  };

private:
  pool_node_t *m_root;
  void *m_buffer;
  size_t m_size;
  size_t m_blk;
};

#endif // POOL_ALLOCATOR_H
