#ifndef MUTILS_H
#define MUTILS_H

#include <cstddef>
#include <cstdint>

using raw = void;
using byte = char;
using marker = uintptr_t;

struct memblk {
  raw *ptr;
  size_t size;
};

struct block_node_t {
  block_node_t *next;
  size_t size;
};

template <size_t blk_size> constexpr size_t align_block(size_t size) {
  return (size + (blk_size - 1)) & ~(blk_size - 1);
}

template <size_t blk_size> constexpr bool is_aligned(const raw *__restrict p) {
  return reinterpret_cast<uintptr_t>(p) % blk_size == 0;
}

template <typename T> constexpr T max(T x, T y) {
  return x ^ ((x ^ y) & -(x < y));
}

constexpr raw *address_add(raw *__restrict p, size_t adjust) {
  return static_cast<byte *>(p) + adjust;
}

constexpr raw *address_sub(raw *__restrict p, size_t adjust) {
  return static_cast<byte *>(p) - adjust;
}

template <typename __T> __T *address_add(__T *__restrict p, size_t adjust) {
  return reinterpret_cast<__T *>(reinterpret_cast<uintptr_t>(p) + adjust);
}

constexpr size_t KB(size_t size) { return size * 1024; }

constexpr size_t MB(size_t size) { return size * 1024 * 1024; }

#endif // MUTILS_H
