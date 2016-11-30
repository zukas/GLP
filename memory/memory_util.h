#ifndef MEMORY_UTIL_H
#define MEMORY_UTIL_H

#include <cstddef>
#include <cstdint>

namespace glp {

struct memblk {
  void *ptr;
  size_t size;
};

using byte = char;
using marker = uintptr_t;

template <long blk_size> constexpr long align_block(long size) {
  return (size + (blk_size - 1)) & ~(blk_size - 1);
}

template <size_t blk_size> constexpr size_t align_block(size_t size) {
  return (size + (blk_size - 1)) & ~(blk_size - 1);
}

template <long blk_size> constexpr bool is_aligned(const void *__restrict p) {
  return reinterpret_cast<uintptr_t>(p) % blk_size == 0;
}

template <typename T> constexpr T max(T x, T y) {
  return x ^ ((x ^ y) & -(x < y));
}
}

#endif // MEMORY_UTIL_H
