#ifndef MESH_STORE_H
#define MESH_STORE_H

#include "mesh.h"

#include <cstddef>
#include <cstdint>

class mesh_store {
public:
  mesh_store();
  mesh_store(size_t count);
  ~mesh_store();

  void store(uint32_t mesh_id, mesh_buffer_gl mesh);
  mesh_buffer_gl get(uint32_t mesh_id);

private:
  mesh_buffer_gl *m_buffers;
  size_t m_count;
};

#endif // MESH_STORE_H
