#ifndef DYNAMIC_BLOCK_ALLOCATOR_H
#define DYNAMIC_BLOCK_ALLOCATOR_H

#include "global_heap_memory.h"
#include <cassert>

template <size_t _S, typename __Ma = global_heap_allocator, size_t _A = 16>
class dynamic_block_allocator : __Ma {
  static_assert(_A >= 16, "Alignment must be at least 16 bytes");
  static_assert(_S > 0, "Size must be more then zero");

public:
  dynamic_block_allocator() : m_buffer(__Ma::alloc(size())) {
    m_root = static_cast<block_node_t *>(m_buffer);
    m_root->next = nullptr;
    m_root->size = _S;
  }

  ~dynamic_block_allocator() {
    __Ma::free(m_buffer, size());
    m_buffer = nullptr;
    m_root = nullptr;
  }

  void *alloc(size_t size) {
    size = align_block<_A>(size);
    void *res = nullptr;
    {
      block_node_t *parent = nullptr;
      block_node_t *node = m_root;
      for (;;) {
        if (node == nullptr || node->size >= size) {
          break;
        }
        parent = node;
        node = node->next;
      }
      assert(node);
      size_t rem = node->size - size;
      assert(rem >= 0);
      block_node_t *next = nullptr;
      if (rem == 0) {
        next = node->next;
      } else {
        next = address_add(node, size);
        next->next = node->next;
        next->size = rem;
      }
      if (parent) {
        parent->next = next;
      } else {
        m_root = next;
      }

      assert(m_root != nullptr);
      res = static_cast<void *>(node);
    }
    return res;
  }

  void free(void *ptr, size_t size) {
    assert(owns(ptr));
    size = align_block<_A>(size);
    block_node_t *node = m_root;

    if (ptr < node || node == nullptr) {
      block_node_t *tmp = static_cast<block_node_t *>(ptr);
      block_node_t *next = nullptr;
      size_t block_size = 0;
      void *adjusted = address_add(ptr, size);
      if (adjusted == node) {
        next = node->next;
        block_size = size + node->size;
      } else {
        next = node;
        block_size = size;
      }
      tmp->next = next;
      tmp->size = block_size;
      m_root = tmp;
    } else {

      block_node_t *next = node->next;
      while (next < ptr) {
        node = next;
        next = next->next;
      }
      size_t block_size = node->size;

      void *end_node = address_add(node, block_size);
      void *end_blk = address_add(ptr, size);

      if (end_node == ptr && end_blk == next) {
        block_size += size + node->next->size;
        next = next->next;
      } else if (end_node == ptr && end_blk < next) {
        block_size += size;
      } else if (end_node < ptr && end_blk == next) {
        block_node_t *tmp = static_cast<block_node_t *>(ptr);
        tmp->next = next->next;
        tmp->size = size + next->size;
        next = tmp;
      } else {
        block_node_t *tmp = static_cast<block_node_t *>(ptr);
        tmp->next = next;
        tmp->size = size;
        next = tmp;
      }

      node->next = next;
      node->size = block_size;
    }
  }

  bool owns(void *ptr) {
    return ptr >= m_buffer && ptr < address_add(m_buffer, size());
  }

  size_t size() const { return _S; }
  size_t align_size(size_t size) const { return align_block<_A>(size); }

private:
  void *m_buffer;
  block_node_t *m_root;
};

#endif // DYNAMIC_BLOCK_ALLOCATOR_H
