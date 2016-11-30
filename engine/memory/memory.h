#ifndef MEMORY_H
#define MEMORY_H

#include "mutils.h"
#include <cstddef>
#include <mutex>

class Memory {
public:
  Memory();
  explicit Memory(size_t size);
  ~Memory();
  void init(size_t size);
  void deinit();
  static Memory &instance();

public:
  memblk alloc(size_t size);
  void free(memblk blk);
  bool owns(memblk blk);

private:
  struct block_node_t {
    block_node_t *next;
    size_t size;
  };

private:
  static Memory *g_self;

  raw *m_buffer;
  size_t m_size;
  block_node_t *m_root;
  std::mutex m_lock;
};

memblk mem_alloc(size_t size);
void mem_free(memblk blk);

#endif // MEMORY_H
