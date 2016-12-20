#include "runtime.h"

#include "audio/audio.h"
#include "interface/interface.h"
#include "io/io.h"
#include "memory/global_heap_memory.h"
#include "memory/global_stack_memory.h"
#include "memory/stack_allocator.h"
#include "phisics/phisics.h"
#include "renderer/renderer.h"
#include "resource/resource.h"

#include "utils/static_instance_factory.h"

Runtime::Runtime() {

  global_heap_init(MB(800));
  global_stack_init(MB(1));

  io = global_stack_make_new<IO>();
  resource = global_stack_make_new<Resource>();
  audio = global_stack_make_new<Audio>();
  render = global_stack_make_new<Render>();
  hud = global_stack_make_new<Interface>();
  phisics = global_stack_make_new<Phisics>();

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

  global_stack_destroy(phisics);
  global_stack_destroy(hud);
  global_stack_destroy(render);
  global_stack_destroy(audio);
  global_stack_destroy(resource);
  global_stack_destroy(io);

  global_stack_deinit();
  global_heap_deinit();
}

void Runtime::run() {

  for (int i = 0; i < 1000; ++i) {
    render->render();
  }
}
