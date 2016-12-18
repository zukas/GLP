#include "io_memory.h"

#include "memory/atomic_pool_allocator.h"
#include "utils/static_instance_factory.h"

using G_IOM =
    static_instance_factory<atomic_pool_allocator<512, 1024, sys_allocator>>;

void io_mem_init() { G_IOM::init(); }

void io_mem_deinit() { G_IOM::deinit(); }

size_t io_mem_buffer_size() { return 512; }

char *io_buffer_alloc() { return static_cast<char *>(G_IOM::get().alloc()); }

void io_buffer_free(char *buffer) { G_IOM::get().free(buffer); }
