#ifndef GLOBAL_HEAP_ALLOCATOR_H
#define GLOBAL_HEAP_ALLOCATOR_H

#include <atomic>
#include <stddef.h>

namespace glp {
namespace mem {
struct heap_data;
class global_heap_allocator {
private:
  static std::atomic<heap_data *> g_heap;

public:
  static void init(size_t size);
  static void deinit();

  static void *mem_alloc(size_t size);
  static void mem_free(void *ptr);
};
}
}
#endif // GLOBAL_HEAP_ALLOCATOR_H
