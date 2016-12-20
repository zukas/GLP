#ifndef MEMORY_H
#define MEMORY_H

#include "mutils.h"

#include <cstddef>

bool global_heap_init(size_t size);
void global_heap_deinit();

size_t global_heap_align_size(size_t size);
raw *global_heap_alloc(size_t size);
void global_heap_free(raw *ptr, size_t size);
bool global_heap_owns(raw *ptr);

struct global_heap_allocator {
  static inline raw *alloc(size_t size) { return global_heap_alloc(size); }
  static inline void free(raw *ptr, size_t size) {
    global_heap_free(ptr, size);
  }
  static inline size_t align_size(size_t size) {
    return global_heap_align_size(size);
  }
};

#endif // MEMORY_H
