#ifndef STACK_ALLOCATOR_H
#define STACK_ALLOCATOR_H

#include "global_heap_memory.h"

template <size_t _S, typename __Ma = global_heap_allocator, size_t _A = 16>
class stack_allocator : __Ma {
public:
  stack_allocator() : m_buffer(__Ma::alloc(_S)), m_cursor(m_buffer) {}
  stack_allocator(const stack_allocator &) = delete;
  stack_allocator(stack_allocator &&) = delete;
  ~stack_allocator() { __Ma::free(m_buffer, _S); }

  void *alloc(size_t size) {
    size = align_block<_A>(size);
    void *ptr = m_cursor;
    m_cursor = address_add(m_cursor, size);
    return ptr;
  }

  void free(void *ptr, size_t size) {
    size = align_block<_A>(size);
    void *tmp = address_sub(ptr, size);
    if (tmp == m_cursor) {
      m_cursor = ptr;
    }
  }

  size_t size() { return _S; }
  size_t align_size(size_t size) { return align_block<_A>(size); }

  void clear() { m_cursor = m_buffer; }

private:
  void *m_buffer;
  void *m_cursor;
};

#endif // STACK_ALLOCATOR_H
