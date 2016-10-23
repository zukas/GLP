#include "allocator.h"

#include <cstdlib>
#include <errno.h>

glp::memblk glp::mem_alloc_sys::alloc(long size) {
  assert(size > 0);
  return memblk{::malloc(static_cast<size_t>(size)), size};
}

void glp::mem_alloc_sys::free(glp::memblk blk) { ::free(blk.ptr); }

glp::byte *glp::__glp_mem_sys_alloc(long size) {
  return size > 0 ? static_cast<glp::byte *>(
                        ::malloc(align_block<64>(static_cast<size_t>(size))))
                  : nullptr;
}

void glp::__glp_mem_sys_free(glp::byte *p) { ::free(p); }
