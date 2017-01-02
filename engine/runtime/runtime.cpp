#include "runtime.h"

#include "audio/audio.h"
#include "interface/interface.h"
#include "io/io.h"
#include "memory/global_heap_memory.h"
#include "memory/global_stack_memory.h"
#include "phisics/phisics.h"
#include "renderer/renderer.h"
#include "resource/resource.h"

bool runtime_init(const engine_description &desc) {
  bool status = global_heap_init(MB(800));
  status = status && global_stack_init(MB(1));

  status = status && io_init();
  status = status && resource_init();
  status = status && audio_init();
  status = status && renderer_init(desc);
  status = status && ui_init();
  status = status && phisics_init();
  return status;
}

void runtime_deinit() {
  phisics_deinit();
  ui_deinit();
  renderer_deinit();
  audio_deinit();
  resource_deinit();
  io_deinit();

  global_stack_deinit();
  global_heap_deinit();
}
