#ifndef MEMORY_H
#define MEMORY_H

#include "mutils.h"
#include "utils/global_instance.h"

#include <cstddef>
#include <mutex>

class Memory : public global_instance<Memory> {
public:
  Memory();
  explicit Memory(size_t size);
  ~Memory();
  void init(size_t size);
  void deinit();

public:
  memblk alloc(size_t size);
  void free(memblk blk);
  bool owns(memblk blk);

private:
  raw *m_buffer;
  size_t m_size;
  block_node_t *m_root;
  std::mutex m_lock;
};

using G_Memory = global_instance<Memory>;

memblk mem_alloc(size_t size);
void mem_free(memblk blk);

struct sys_allocator {
  static memblk acquire(size_t size) { return mem_alloc(size); }
  static void release(memblk blk) { mem_free(blk); }
};

#endif // MEMORY_H
