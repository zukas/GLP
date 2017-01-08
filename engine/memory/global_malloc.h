#ifndef GLOBAL_MALLOC_H
#define GLOBAL_MALLOC_H

#include <cstddef>
#include <cstdint>

#ifdef ALLOCATOR_DEBUG
void *debug_glp_malloc(size_t size, const char *filename, const char *function,
                       uint32_t line);
#define glp_malloc(size)                                                       \
  debug_glp_malloc(size, __FILE__, __FUNCTION__, __LINE__)
#else
void *glp_malloc(size_t size);
#endif

void glp_free(void *ptr);
void *glp_realloc(void *ptr, size_t size);

#endif // GLOBAL_MALLOC_H
