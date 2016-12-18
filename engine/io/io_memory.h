#ifndef IO_MEMORY_H
#define IO_MEMORY_H

#include <cassert>
#include <cstddef>
#include <new>

void io_mem_init();
void io_mem_deinit();

size_t io_mem_buffer_size();
char *io_buffer_alloc();
void io_buffer_free(char *buffer);

#endif // IO_MEMORY_H
