#include "runtime_memory.h"

#include "memory/stack_allocator.h"
#include "utils/static_instance_factory.h"

using G_RM = static_instance_factory<stack_allocator<MB(2), sys_allocator, 64>>;

void runtime_mem_init() { G_RM::init(); }

void runtime_mem_deinit() { G_RM::deinit(); }

void *runtime_mem_alloc(size_t size) { return G_RM::get().alloc(size).ptr; }

void runtime_mem_free(void *ptr, size_t size) {
  auto &tmp = G_RM::get();
  size = tmp.opt_size(size);
  G_RM::get().free({ptr, size});
}
