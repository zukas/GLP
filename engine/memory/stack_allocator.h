#ifndef STACK_ALLOCATOR_H
#define STACK_ALLOCATOR_H

#include "memory.h"

template <size_t _S, typename __Ma = sys_allocator> class stack_allocator {
public:
  stack_allocator() : m_blk(__Ma::acquire(_S)), m_cursor(m_blk.ptr) {}
  stack_allocator(const stack_allocator &) = delete;
  stack_allocator(stack_allocator &&) = delete;
  ~stack_allocator() { __Ma::release(m_blk); }

  memblk alloc(size_t size) {
    size = align_block<16>(size);
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

  void clear() { m_cursor = m_blk.ptr; }

private:
  memblk m_blk;
  raw *m_cursor;
};

#endif // STACK_ALLOCATOR_H
