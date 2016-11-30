#include "global_heap_allocator.h"

#include <atomic>
#include <cassert>
#include <cstdlib>

#include "memory_util.h"

namespace glp {
namespace mem {

struct heap_data_node {
  std::atomic<heap_data_node *> next;
  size_t size;
};

struct heap_data {
  char *m_buffer;
  std::atomic_size_t m_idx;
  size_t m_size;
  std::atomic<heap_data_node *> m_root;
};
}
}

std::atomic<glp::mem::heap_data *> glp::mem::global_heap_allocator::g_heap{
    nullptr};

void glp::mem::global_heap_allocator::init(size_t size) {
  assert(g_heap == nullptr);

  size_t aligned_struct = align_block<64>(sizeof(heap_data));
  size_t aligned_size = align_block<64>(size);
  void *buff = ::malloc(aligned_struct + aligned_size);

  heap_data *inst = static_cast<heap_data *>(buff);
  inst->m_buffer = static_cast<char *>(buff) + aligned_struct;
  inst->m_idx.store(0);
  inst->m_size = aligned_size;
  heap_data *expect = nullptr;
  if (!std::atomic_compare_exchange_strong(&g_heap, &expect, inst)) {
    ::free(buff);
  }
}

void glp::mem::global_heap_allocator::deinit() {
  assert(g_heap != nullptr);

  size_t aligned_struct = align_block<64>(sizeof(heap_data));
  heap_data *expect = nullptr;
  heap_data *inst = std::atomic_exchange(&g_heap, expect);
  if (inst != nullptr) {
    ::free(inst->m_buffer - aligned_struct);
  }
}

void *glp::mem::global_heap_allocator::mem_alloc(size_t size) {

  auto heap = g_heap.load(std::memory_order_acquire);
  assert(heap != nullptr);

  char *buff = nullptr;
  size_t alignd_size = align_block<64>(size);
  size_t c_idx = heap->m_idx.load(std::memory_order_acquire);
  size_t n_idx = c_idx + alignd_size;
  size_t total = heap->m_size;

  while (n_idx <= total &&
         !heap->m_idx.compare_exchange_strong(c_idx, n_idx,
                                              std::memory_order_release,
                                              std::memory_order_acquire)) {
    n_idx = c_idx + alignd_size;
  }

  if (n_idx <= total) {
    buff = &heap->m_buffer[c_idx];
  }

  return buff;
}

void glp::mem::global_heap_allocator::mem_free(void *ptr) {}
