#include "io_memory.h"

#include "memory/atomic_pool_allocator.h"
#include "memory/type_allocator.h"

struct {
  type_allocator<char, atomic_pool_allocator> io_buffer;
} __context;

bool io_buffers_init() { __context.io_buffer.init(IO_BUFFER_SIZE, IO_BACKLOG); }

void io_buffers_deinit() { __context.io_buffer.deinit(); }

char *io_buffers_get() { return __context.io_buffer.make_new(); }

void io_buffer_release(char *ptr) { __context.io_buffer.destroy(ptr); }
