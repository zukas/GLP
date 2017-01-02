#include "global_malloc.h"

#include "global_stack_memory.h"

#include <cassert>
#include <cstring>

struct malloc_header {
  uint32_t block_size;
  uint32_t used_size;
};

void *glp_malloc(size_t size) {
  size_t aligned_size = global_stack_align_size(size + sizeof(malloc_header));
  void *ptr = global_stack_alloc(aligned_size);
  malloc_header *header = static_cast<malloc_header *>(ptr);
  header->block_size = aligned_size - sizeof(malloc_header);
  header->used_size = size;
  return address_add(ptr, sizeof(malloc_header));
}

void glp_free(void *ptr) {
  if (ptr) {
    malloc_header *header =
        static_cast<malloc_header *>(address_sub(ptr, sizeof(malloc_header)));
    global_stack_free(header, header->block_size);
  }
}

void *glp_realloc(void *ptr, size_t size) {
  if (size == 0) {
    glp_free(ptr);
    return nullptr;
  } else {
    assert(ptr);
    malloc_header *header =
        static_cast<malloc_header *>(address_sub(ptr, sizeof(malloc_header)));
    if (header->block_size >= size) {
      return ptr;
    } else {
      void *new_ptr = malloc(size);
      memcpy(new_ptr, ptr, header->used_size);
      global_stack_free(header, header->block_size);
      return new_ptr;
    }
  }
}
