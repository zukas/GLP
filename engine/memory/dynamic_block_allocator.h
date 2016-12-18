#ifndef DYNAMIC_BLOCK_ALLOCATOR_H
#define DYNAMIC_BLOCK_ALLOCATOR_H

#include "memory.h"
#include <cassert>

template <size_t _S, typename __Ma = sys_allocator, size_t _A = 16>
class dynamic_block_allocator {
  static_assert(_A >= 16, "Alignment must be at least 16 bytes");
  static_assert(_S > 0, "Size must be more then zero");

protected:
  using t_type = memblk;

public:
  dynamic_block_allocator() : m_blk(__Ma::acquire(_S)) {
    m_root = static_cast<block_node_t *>(m_blk.ptr);
    m_root->next = nullptr;
    m_root->size = _S;
  }

  ~dynamic_block_allocator() {
    __Ma::release(m_blk);
    m_blk = {nullptr, 0};
    m_root = nullptr;
  }

  memblk alloc(size_t size) {
    size = align_block<_A>(size);
    raw *res = nullptr;
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
      res = static_cast<raw *>(node);
    }
    return {res, size};
  }

  void free(memblk blk) {
    assert(owns(blk));

    block_node_t *node = m_root;

    if (blk.ptr < node || node == nullptr) {
      block_node_t *tmp = static_cast<block_node_t *>(blk.ptr);
      block_node_t *next = nullptr;
      size_t block_size = 0;
      raw *adjusted = address_add(blk.ptr, blk.size);
      if (adjusted == node) {
        next = node->next;
        block_size = blk.size + node->size;
      } else {
        next = node;
        block_size = blk.size;
      }
      tmp->next = next;
      tmp->size = block_size;
      m_root = tmp;
    } else {

      block_node_t *next = node->next;
      while (next < blk.ptr) {
        node = next;
        next = next->next;
      }
      size_t block_size = node->size;

      raw *end_node = address_add(node, block_size);
      raw *end_blk = address_add(blk.ptr, blk.size);

      if (end_node == blk.ptr && end_blk == next) {
        block_size += blk.size + node->next->size;
        next = next->next;
      } else if (end_node == blk.ptr && end_blk < next) {
        block_size += blk.size;
      } else if (end_node < blk.ptr && end_blk == next) {
        block_node_t *tmp = static_cast<block_node_t *>(blk.ptr);
        tmp->next = next->next;
        tmp->size = blk.size + next->size;
        next = tmp;
      } else {
        block_node_t *tmp = static_cast<block_node_t *>(blk.ptr);
        tmp->next = next;
        tmp->size = blk.size;
        next = tmp;
      }

      node->next = next;
      node->size = block_size;
    }
  }

  bool owns(memblk blk) {
    return blk.ptr >= m_blk.ptr &&
           blk.ptr <= address_add(m_blk.ptr, m_blk.size - blk.size - 1);
  }

  constexpr static size_t size() { return _S; }
  constexpr static size_t opt_size(size_t size) {
    return align_block<_A>(size);
  }

private:
  memblk m_blk;
  block_node_t *m_root;
};

#endif // DYNAMIC_BLOCK_ALLOCATOR_H
