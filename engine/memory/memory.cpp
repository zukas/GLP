#include "memory.h"

#include <cassert>
#include <cstdlib>

Memory::Memory() : m_buffer(nullptr), m_size(0), m_root(nullptr) {}

Memory::Memory(size_t size) : m_buffer(nullptr) { init(size); }

Memory::~Memory() {}

void Memory::init(size_t size) {

  size = align_block<MB(1)>(size);

  std::lock_guard<std::mutex> _lock(m_lock);
  assert(m_buffer == nullptr);

  m_buffer = ::malloc(size);
  m_size = size;
  m_root = static_cast<block_node_t *>(m_buffer);
  m_root->next = nullptr;
  m_root->size = size;
}

void Memory::deinit() {
  std::lock_guard<std::mutex> _lock(m_lock);
  ::free(m_buffer);
  m_buffer = nullptr;
  m_size = 0;
  m_root = nullptr;
}

memblk Memory::alloc(size_t size) {

  size = align_block<MB(1)>(size);
  raw *res = nullptr;

  {
    std::lock_guard<std::mutex> _lock(m_lock);

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

void Memory::free(memblk blk) {
  assert(owns(blk));
  std::lock_guard<std::mutex> _lock(m_lock);

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

bool Memory::owns(memblk blk) {
  return blk.ptr >= m_buffer &&
         blk.ptr <= address_add(m_buffer, m_size - blk.size - 1);
}

memblk mem_alloc(size_t size) { return G_Memory::get()->alloc(size); }

void mem_free(memblk blk) { G_Memory::get()->free(blk); }
