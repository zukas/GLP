#ifndef BUFFERED_ALLOCATOR_H
#define BUFFERED_ALLOCATOR_H

#include "mutils.h"

template <typename _Al, size_t _Num> class buffered_allocator {
  static_assert(_Num > 1, "Buffered allocator expects more then one buffer");

public:
  buffered_allocator() : m_curr(&m_alloc[0]) {}

  ~buffered_allocator() {}
  buffered_allocator(const buffered_allocator &) = delete;
  buffered_allocator(buffered_allocator &&) = delete;

  void *alloc(size_t size) { return m_curr->alloc(size); }

  void swap() {
    if (m_curr == &m_alloc[_Num - 1]) {
      m_curr = &m_alloc[0];
    } else {
      m_curr = address_add(m_curr, sizeof(_Al));
    }
    m_curr->clear();
  }

  size_t size() { return m_curr->size(); }
  size_t align_size(size_t size) { return m_curr->align_size(size); }

private:
  _Al m_alloc[_Num];
  _Al *m_curr;
};

#endif // BUFFERED_ALLOCATOR_H
