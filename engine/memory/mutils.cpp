#include "mutils.h"

#include <cstring>

void mem_clear(void *__restrict ptr, size_t size) { memset(ptr, 0, size); }
