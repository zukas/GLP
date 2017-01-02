#ifndef GLOBAL_STACK_MEMORY_H
#define GLOBAL_STACK_MEMORY_H

#include "mutils.h"
#include "type_allocator.h"

#include <cstddef>

bool global_stack_init(size_t size);
void global_stack_deinit();

size_t global_stack_align_size(size_t size);
void *global_stack_alloc(size_t size);
void global_stack_free(void *ptr, size_t size);
bool global_stack_owns(void *ptr);

struct global_stack_allocator {
  static inline void *alloc(size_t size) { return global_stack_alloc(size); }
  static inline void free(void *ptr, size_t size) {
    global_stack_free(ptr, size);
  }
  static inline size_t align_size(size_t size) {
    return global_stack_align_size(size);
  }
};

template <typename _Tp>
using global_stack_type_allocator = type_allocator<_Tp, global_stack_allocator>;

template <typename _Tp, typename... _Args>
_Tp *global_stack_make_new(_Args... args) {
  return make_new<_Tp, global_stack_allocator, _Args...>(
      std::forward<_Args>(args)...);
}

template <typename _Tp> void global_stack_destroy(_Tp *ptr) {
  destroy<_Tp, global_stack_allocator>(ptr);
}

#endif // GLOBAL_STACK_MEMORY_H
