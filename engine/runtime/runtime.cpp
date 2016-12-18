#include "runtime.h"

#include "audio/audio.h"
#include "interface/interface.h"
#include "io/io.h"
#include "memory/memory.h"
#include "memory/stack_allocator.h"
#include "phisics/phisics.h"
#include "renderer/renderer.h"
#include "resource/resource.h"
#include "runtime_memory.h"

#include "utils/static_instance_factory.h"

using G_HEAP = static_instance_factory<Memory>;

Runtime::Runtime() {

  G_HEAP::init().init(MB(800));

  runtime_mem_init();

  io = runtime_new<IO>();
  resource = runtime_new<Resource>();
  audio = runtime_new<Audio>();
  render = runtime_new<Render>();
  hud = runtime_new<Interface>();
  phisics = runtime_new<Phisics>();

  bool status = true;
  status = status && io->init();
  status = status && resource->init();
  status = status && audio->init();
  status = status && render->initOpenGL();
  status = status && hud->init();
  status = status && phisics->init();
}

Runtime::~Runtime() {

  phisics->deinit();
  hud->deinit();
  render->deinit();
  audio->deinit();
  resource->deinit();
  io->deinit();

  runtime_delete(phisics);
  runtime_delete(hud);
  runtime_delete(render);
  runtime_delete(audio);
  runtime_delete(resource);
  runtime_delete(io);

  runtime_mem_deinit();

  G_HEAP::get().deinit();
  G_HEAP::deinit();
}

void Runtime::run() {

  for (int i = 0; i < 1000; ++i) {
    render->render();
  }
}
