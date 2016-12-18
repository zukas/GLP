#ifndef RUNTIME_MEMORY_H
#define RUNTIME_MEMORY_H

#include <cassert>
#include <cstddef>
#include <new>

void runtime_mem_init();
void runtime_mem_deinit();

void *runtime_mem_alloc(size_t size);
void runtime_mem_free(void *ptr, size_t size);

template <typename _Tp> _Tp *runtime_new(size_t size = sizeof(_Tp)) {
  assert(size >= sizeof(_Tp));
  if (std::is_trivially_constructible<_Tp>::value) {
    return static_cast<_Tp *>(runtime_mem_alloc(size));
  } else {
    return new (runtime_mem_alloc(size)) _Tp();
  }
}

template <typename _Tp>
void runtime_delete(_Tp *ptr, size_t size = sizeof(_Tp)) {
  assert(ptr);
  ptr->~_Tp();
  assert(size >= sizeof(_Tp));
  runtime_mem_free(ptr, size);
}

#endif // RUNTIME_MEMORY_H
