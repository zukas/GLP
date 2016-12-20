#include "backend.h"

#include "utils/static_instance_factory.h"

#include <cstring>

#ifdef linux

#include <dlfcn.h>

#define GL_LIB "libbackend-gl.so"
#define VK_LIB "libbackend-vk.so"

void load_backend(const char *__restrict name,
                  backend_mapper *__restrict mapper) {
  void *handle = dlopen(name, RTLD_LAZY);
  if (handle) {
    mapper->handle = handle;
    mapper->init = reinterpret_cast<init_fn>(dlsym(handle, "init"));
    mapper->deinit = reinterpret_cast<deinit_fn>(dlsym(handle, "deinit"));
    mapper->begin_frame =
        reinterpret_cast<begin_frame_fn>(dlsym(handle, "begin_frame"));
    mapper->end_frame =
        reinterpret_cast<begin_frame_fn>(dlsym(handle, "end_frame"));
    mapper->init_mesh_store =
        reinterpret_cast<init_mesh_store_fn>(dlsym(handle, "init_mesh_store"));
    mapper->create_mesh =
        reinterpret_cast<create_mesh_fn>(dlsym(handle, "create_mesh"));
    mapper->destroy_mesh =
        reinterpret_cast<destroy_mesh_fn>(dlsym(handle, "destroy_mesh"));
    mapper->render_mesh =
        reinterpret_cast<render_mesh_fn>(dlsym(handle, "render_mesh"));
  }
}

void unload_backend(backend_mapper *mapper) {
  if (mapper->handle) {
    dlclose(mapper->handle);
    memset(mapper, 0, sizeof(backend_mapper));
  }
}

#endif

Backend::Backend() {
  m_mapper = &static_instance_factory<backend_mapper>::init();
  m_type = None;
}

Backend::~Backend() {
  // unload library
  static_instance_factory<backend_mapper>::deinit();
  m_mapper = nullptr;
  m_type = None;
}

void Backend::load(Backend::BackendType type) {
  if (type != m_type) {
    unload_backend(m_mapper);
    switch (type) {
    case OpengGL:
      load_backend(GL_LIB, m_mapper);
      break;
    case Vulkan:
      load_backend(VK_LIB, m_mapper);
      break;
    case None:
      break;
    }
    m_type = type;
  }
}

Backend::BackendType Backend::type() { return m_type; }
