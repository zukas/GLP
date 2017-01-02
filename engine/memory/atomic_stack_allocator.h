#ifndef ATOMIC_STACK_ALLOCATOR_H
#define ATOMIC_STACK_ALLOCATOR_H

#include "global_heap_memory.h"

#include <atomic>

template <size_t _S, typename __Ma = global_heap_allocator, size_t _A = 16>
class atomic_stack_allocator : __Ma {
public:
  atomic_stack_allocator() : m_buffer(__Ma::alloc(_S)), m_cursor(m_buffer) {}
  atomic_stack_allocator(const atomic_stack_allocator &) = delete;
  atomic_stack_allocator(atomic_stack_allocator &&) = delete;
  ~atomic_stack_allocator() { __Ma::free(m_buffer, _S); }

  void *alloc(size_t size) {
    size = align_block<_A>(size);
    void *ptr = m_cursor.load(std::memory_order_acquire);
    void *next = address_add(ptr, size);
    while (!m_cursor.compare_exchange_strong(ptr, next,
                                             std::memory_order_acq_rel)) {
      next = address_add(ptr, size);
    }
    return ptr;
  }

  void free(void *ptr, size_t size) {
    size = align_block<_A>(size);
    void *tmp = address_sub(ptr, size);
    m_cursor.compare_exchange_strong(tmp, ptr, std::memory_order_acq_rel);
  }

  size_t size() { return _S; }
  size_t align_size(size_t size) { return align_block<_A>(size); }

  void clear() { m_cursor.store(m_buffer); }

private:
  void *m_buffer;
  std::atomic<void *> m_cursor;
};

#endif // ATOMIC_STACK_ALLOCATOR_H
