#ifndef MUTILS_H
#define MUTILS_H

#include <cstddef>
#include <cstdint>

using byte = char;
using marker = uintptr_t;

struct memblk {
  void *ptr;
  size_t size;
};

struct block_node_t {
  block_node_t *next;
  size_t size;
};

#ifdef ALLOCATOR_DEBUG

struct locator_info {
  const char *filename;
  const char *function;
  uint32_t line;
  size_t size;
  void *ptr;
  locator_info *next;
};

#endif

template <size_t blk_size> constexpr size_t align_block(size_t size) {
  return (size + (blk_size - 1)) & ~(blk_size - 1);
}

template <size_t blk_size> constexpr bool is_aligned(const void *__restrict p) {
  return reinterpret_cast<uintptr_t>(p) % blk_size == 0;
}

constexpr void *address_add(void *__restrict p, size_t adjust) {
  return static_cast<byte *>(p) + adjust;
}

constexpr void *address_sub(void *__restrict p, size_t adjust) {
  return static_cast<byte *>(p) - adjust;
}

template <typename __T> __T *address_add(__T *__restrict p, size_t adjust) {
  return reinterpret_cast<__T *>(reinterpret_cast<uintptr_t>(p) + adjust);
}

template <typename _Tp, typename _S>
constexpr size_t array_size_bytes(_S size) {
  return static_cast<size_t>(size) * sizeof(_Tp);
}

constexpr size_t KB(size_t size) { return size * 1024; }

constexpr size_t MB(size_t size) { return size * 1024 * 1024; }

void mem_clear(void *__restrict ptr, size_t size);

#endif // MUTILS_H
