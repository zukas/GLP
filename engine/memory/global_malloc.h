#ifndef GLOBAL_MALLOC_H
#define GLOBAL_MALLOC_H

#include <cstddef>

void *glp_malloc(size_t size);
void glp_free(void* ptr);
void* glp_realloc(void *ptr, size_t size);

#endif // GLOBAL_MALLOC_H
