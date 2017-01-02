#ifndef ATOMIC_ARRAY_H
#define ATOMIC_ARRAY_H

#include "global_heap_memory.h"

#include <atomic>
#include <cassert>

template <typename _Tp, size_t _Num, typename __Ma = global_heap_allocator,
          size_t _A = sizeof(void *)>
class atomic_array : __Ma {
public:
  using type = _Tp *;

public:
  atomic_array()
      : m_buffer(static_cast<type>(__Ma::alloc(sizeof(_Tp)))),
        m_cursor(m_buffer) {}
  atomic_array(const atomic_array &) = delete;
  atomic_array(atomic_array &&) = delete;
  ~atomic_array() { __Ma::free(m_buffer, sizeof(_Tp)); }

  type aquire() {
    size_t size = align_block<_A>(sizeof(_Tp));
    type ptr = m_cursor.load(std::memory_order_acquire);
    type next = address_add(ptr, size);
    while (!m_cursor.compare_exchange_strong(ptr, next,
                                             std::memory_order_acq_rel)) {
      next = address_add(ptr, size);
    }
    return ptr;
  }

  void release(type ptr) {
    type tmp = address_sub(ptr, align_block<_A>(sizeof(_Tp)));
    m_cursor.compare_exchange_strong(tmp, ptr, std::memory_order_acq_rel);
  }

  constexpr size_t capacity() const {
    return align_block<_A>(sizeof(_Tp)) * _Num;
  }
  size_t size() const {
    type curr = m_cursor.load(std::memory_order_acquire);
    ptrdiff_t diff = curr - m_buffer;
    return static_cast<size_t>(diff);
  }

  type operator[](long idx) const {
    assert(size() > idx);
    return m_buffer[idx];
  }

  const type *data() const { return m_buffer; }

  void clear() { m_cursor.store(m_buffer); }

private:
  type m_buffer;
  std::atomic<type> m_cursor;
};

#endif // ATOMIC_ARRAY_H
