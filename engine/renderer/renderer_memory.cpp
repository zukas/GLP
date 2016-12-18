#include "renderer_memory.h"

#include "memory/dynamic_block_allocator.h"
#include "utils/static_instance_factory.h"

using G_REM = static_instance_factory<dynamic_block_allocator<MB(16)>>;

void renderer_mem_init() { G_REM::init(); }

void renderer_mem_deinit() { G_REM::deinit(); }

void *renderer_mem_alloc(size_t size) { return G_REM::get().alloc(size).ptr; }

void renderer_mem_free(void *ptr, size_t size) {
  auto &tmp = G_REM::get();
  size = tmp.opt_size(size);
  G_REM::get().free({ptr, size});
}

memblk renderer_allocator::acquire(size_t size) {
  return G_REM::get().alloc(size);
}

void renderer_allocator::release(memblk blk) { G_REM::get().free(blk); }
