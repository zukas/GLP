#ifndef IO_MEMORY_H
#define IO_MEMORY_H

#include "memory/type_allocator.h"

#ifndef IO_BUFFER_SIZE
#define IO_BUFFER_SIZE 512
#endif

#ifndef IO_BACKLOG
#define IO_BACKLOG 1024
#endif

bool io_buffers_init();
void io_buffers_deinit();

char *io_buffers_get();
void io_buffer_release(char *ptr);

#endif // IO_MEMORY_H
