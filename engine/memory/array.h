#ifndef ARRAY_H
#define ARRAY_H

#include "global_heap_memory.h"

template <typename _Tp, size_t _Num, typename __Ma = global_heap_allocator,
          size_t _A = sizeof(void *)>
class array : __Ma {
public:
  using type = _Tp *;

public:
  array()
      : m_buffer(static_cast<type>(__Ma::alloc(sizeof(_Tp)))),
        m_cursor(m_buffer) {}
  array(const array &) = delete;
  array(array &&) = delete;
  ~array() { __Ma::free(m_buffer, sizeof(_Tp)); }

  type aquire() {
    type ptr = m_cursor;
    m_cursor = address_add(m_cursor, align_block<_A>(sizeof(_Tp)));
    return ptr;
  }

  void release(type ptr) {
    type tmp = address_sub(ptr, align_block<_A>(sizeof(_Tp)));
    if (tmp == m_cursor) {
      m_cursor = ptr;
    }
  }

  constexpr size_t capacity() const {
    return align_block<_A>(sizeof(_Tp)) * _Num;
  }
  size_t size() const {
    ptrdiff_t diff = m_cursor - m_buffer;
    return static_cast<size_t>(diff);
  }

  type operator[](long idx) const {
    assert(size() > idx);
    return m_buffer[idx];
  }

  void clear() { m_cursor = m_buffer; }

private:
  type m_buffer;
  type m_cursor;
};

#endif // ARRAY_H
