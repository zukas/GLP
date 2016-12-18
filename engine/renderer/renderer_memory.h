#ifndef RENDERER_MEMORY_H
#define RENDERER_MEMORY_H

#include "memory/mutils.h"

#include <cassert>
#include <cstddef>
#include <new>

void renderer_mem_init();
void renderer_mem_deinit();

void *renderer_mem_alloc(size_t size);
void renderer_mem_free(void *ptr, size_t size);

template <typename _Tp> _Tp *renderer_new(size_t size = sizeof(_Tp)) {
  assert(size >= sizeof(_Tp));
  if (std::is_trivially_constructible<_Tp>::value) {
    return static_cast<_Tp *>(renderer_mem_alloc(size));
  } else {
    return new (renderer_mem_alloc(size)) _Tp();
  }
}

template <typename _Tp>
void renderer_delete(_Tp *ptr, size_t size = sizeof(_Tp)) {
  assert(ptr);
  if (!std::is_trivially_destructible<_Tp>::value) {
    ptr->~_Tp();
  }
  assert(size >= sizeof(_Tp));
  renderer_mem_free(ptr, size);
}

struct renderer_allocator {
  static memblk acquire(size_t size);
  static void release(memblk blk);
};

#endif // RENDERER_MEMORY_H
