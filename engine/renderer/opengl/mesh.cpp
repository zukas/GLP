#include "mesh.h"

#include "gl_headers.h"
#include "mesh_store.h"
#include "renderer/renderer_memory.h"

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
  mesh_store_gl_store(mesh_id, i_size, buffer);
  return mesh_id;
}

void destroy_mesh_gl(uint32_t mesh_id) {
  mesh_buffer_gl buffer = mesh_store_gl_get_buffers(mesh_id);
  glDeleteBuffers(2, buffer.data);
  glDeleteVertexArrays(1, &mesh_id);
}

void render_meshes_gl(const uint32_t *mesh_ids, long size) {

  long *elems = renderer_make_array<long>(size);
  mesh_store_gl_get_elements(mesh_ids, elems, size);
  for (long i = 0; i < size; i++) {
    glBindVertexArray(mesh_ids[i]);
    glDrawElements(GL_TRIANGLES, elems[i], GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
  }
  renderer_destroy_array(elems, size);
}
