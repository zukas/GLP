#ifndef STACK_ALLOCATOR_H
#define STACK_ALLOCATOR_H

#include "memory.h"

template <size_t _S, typename __Ma = sys_allocator, size_t _A = 16>
class stack_allocator {
protected:
  using t_type = memblk;

public:
  stack_allocator() : m_blk(__Ma::acquire(_S)), m_cursor(m_blk.ptr) {}
  stack_allocator(const stack_allocator &) = delete;
  stack_allocator(stack_allocator &&) = delete;
  ~stack_allocator() { __Ma::release(m_blk); }

  memblk alloc(size_t size) {
    size = align_block<_A>(size);
    raw *ptr = m_cursor;
    m_cursor = address_add(m_cursor, size);
    return {ptr, size};
  }

  void free(memblk blk) {
    raw *ptr = address_sub(blk.ptr, blk.size);
    if (ptr == m_cursor) {
      m_cursor = blk.ptr;
    }
  }

  constexpr static size_t size() { return _S; }
  constexpr static size_t opt_size(size_t size) {
    return align_block<_A>(size);
  }

  void clear() { m_cursor = m_blk.ptr; }

private:
  memblk m_blk;
  raw *m_cursor;
};

#endif // STACK_ALLOCATOR_H
