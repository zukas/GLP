#include "renderer_memory.h"

#include "memory/dynamic_block_allocator.h"
#include "memory/global_stack_memory.h"

using allocator = dynamic_block_allocator<MB(16)>;

static allocator *g_renderer{nullptr};

void renderer_mem_init() { g_renderer = global_stack_make_new<allocator>(); }

void renderer_mem_deinit() {
  global_stack_destroy(g_renderer);
  g_renderer = nullptr;
}

void *renderer_mem_alloc(size_t size) { return g_renderer->alloc(size); }

void renderer_mem_free(void *ptr, size_t size) { g_renderer->free(ptr, size); }

size_t renderer_mem_align_size(size_t size) {
  return g_renderer->align_size(size);
}
