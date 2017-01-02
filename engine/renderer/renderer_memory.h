#ifndef RENDERER_MEMORY_H
#define RENDERER_MEMORY_H

#include "memory/type_allocator.h"

void renderer_mem_init();
void renderer_mem_deinit();

void *renderer_mem_alloc(size_t size);
void renderer_mem_free(void *ptr, size_t size);
size_t renderer_mem_align_size(size_t size);

struct renderer_allocator {
  static inline void *alloc(size_t size) { return renderer_mem_alloc(size); }
  static inline void free(void *ptr, size_t size) {
    renderer_mem_free(ptr, size);
  }
  static inline size_t align_size(size_t size) {
    return renderer_mem_align_size(size);
  }
};

template <typename _Tp, typename _S> _Tp *renderer_make_array(_S size) {
  return static_cast<_Tp *>(
      renderer_mem_alloc(sizeof(_Tp) * static_cast<size_t>(size)));
}

template <typename _Tp, typename _S>
void renderer_destroy_array(_Tp *ptr, _S size) {
  renderer_mem_free(ptr, sizeof(_Tp) * static_cast<size_t>(size));
}

template <typename _Tp, typename... _Args>
_Tp *renderer_make_new(_Args... args) {
  return make_new<_Tp, renderer_allocator>(std::forward<_Args>(args)...);
}

template <typename _Tp> void renderer_destroy(_Tp *ptr) {
  destroy<_Tp, renderer_allocator>(ptr);
}

template <typename _Tp>
using renderer_type_allocator = type_allocator<_Tp, renderer_allocator>;

#endif // RENDERER_MEMORY_H
