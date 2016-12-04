#include "engine.h"

#include "audio/audio.h"
#include "interface/interface.h"
#include "io/io.h"
#include "memory/memory.h"
#include "phisics/phisics.h"
#include "render/render.h"
#include "resource/resource.h"
#include "utils/sysdef.h"

#include <cassert>

struct EngineInternal {
  Memory memory;
  IO io;
  Resource resource;
  Audio audio;
  Phisics phisics;
  Render render;
  Interface ui;
};

Engine::Engine() {

  static char _internal[sizeof(EngineInternal)];
  m_internal = new (_internal) EngineInternal();

  m_internal->memory.init(MB(800));
  m_internal->io.init();
  m_internal->resource.init();
  m_internal->audio.init();
  m_internal->render.init();
  m_internal->ui.init();
  m_internal->phisics.init();
}

Engine::~Engine() {
  m_internal->phisics.deinit();
  m_internal->ui.deinit();
  m_internal->render.deinit();
  m_internal->audio.deinit();
  m_internal->resource.deinit();
  m_internal->io.deinit();
  m_internal->memory.deinit();
}

void Engine::run() {

  while (true) {
  }
}
