#ifndef POOL_ALLOCATOR_H
#define POOL_ALLOCATOR_H

#include <cstdlib>

namespace glp {
namespace mem {
template <long capacity, long blk_size> class pool_allocator {
private:
  char *m_buffer;
  long m_idx;
  struct pool_node_t {
    pool_node_t *next;
  } * m_root;

public:
  pool_allocator()
      : m_buffer(static_cast<char *>(::malloc(static_cast<size_t>(capacity)))),
        m_idx(0), m_root(nullptr) {}
  ~pool_allocator() { ::free(m_buffer); }
  void *alloc() {
    void *res = nullptr;
    long tmp = m_idx + blk_size;
    if (tmp <= capacity) {
      res = &m_buffer[m_idx];
      m_idx = tmp;
    } else if (m_root != nullptr) {
      res = m_root;
      m_root = m_root->next;
    }
    return res;
  }
  void free(void *ptr) {
    long tmp = m_idx - blk_size;
    if (&m_buffer[tmp] == ptr) {
      m_idx = tmp;
    } else {
      pool_node_t *blk = static_cast<pool_node_t *>(ptr);
      blk->next = m_root;
      m_root = blk;
    }
  }
};
template <long capacity> using pool_allocator_64 = pool_allocator<capacity, 64>;
}
}

#endif // POOL_ALLOCATOR_H
