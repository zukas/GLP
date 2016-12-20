#include "mesh.h"

#include "gl_headers.h"
#include "mesh_store.h"
#include "utils/static_instance_factory.h"

using G_MS = static_instance_factory<mesh_store>;

void init_mesh_store_gl(size_t count) { G_MS::init(count); }

uint32_t create_mesh_gl(const vertex *__restrict vertexes, long v_size,
                        const uint32_t *__restrict indexes, long i_size) {
  uint32_t mesh_id;

  mesh_buffer_gl buffer;

  glGenVertexArrays(1, &mesh_id);

  glBindVertexArray(mesh_id);
  glGenBuffers(2, buffer.data);
  glBindBuffer(GL_ARRAY_BUFFER, buffer.data[0]);

  glBufferData(GL_ARRAY_BUFFER, v_size * static_cast<long>(sizeof(vertex)),
               vertexes, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0,                // attribute
                        3,                // size
                        GL_FLOAT,         // type
                        GL_FALSE,         // normalized?
                        sizeof(vertexes), // stride
                        nullptr           // array buffer offset
                        );

  glEnableVertexAttribArray(1);
  glVertexAttribPointer(
      1,                                          // attribute
      3,                                          // size
      GL_FLOAT,                                   // type
      GL_TRUE,                                    // normalized?
      sizeof(vertex),                             // stride
      reinterpret_cast<void *>(sizeof(float) * 3) // array buffer offset
      );

  glEnableVertexAttribArray(2);
  glVertexAttribPointer(
      2,                                          // attribute
      2,                                          // size
      GL_FLOAT,                                   // type
      GL_FALSE,                                   // normalized?
      sizeof(vertex),                             // stride
      reinterpret_cast<void *>(sizeof(float) * 6) // array buffer offset
      );

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer.data[1]);

  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               i_size * static_cast<long>(sizeof(uint32_t)), indexes,
               GL_STATIC_DRAW);
  glBindVertexArray(0);
  G_MS::get().store(mesh_id, i_size, buffer);
  return mesh_id;
}

void destroy_mesh_gl(uint32_t mesh_id) {
  mesh_buffer_gl buffer = G_MS::get().get_buffers(mesh_id);
  glDeleteBuffers(2, buffer.data);
  glDeleteVertexArrays(1, &mesh_id);
}

void render_mesh_gl(uint32_t mesh_id) {
  long elems = G_MS::get().get_elem_count(mesh_id);
  glBindVertexArray(mesh_id);
  glDrawElements(GL_TRIANGLES, elems, GL_UNSIGNED_INT, nullptr);
  glBindVertexArray(0);
}
