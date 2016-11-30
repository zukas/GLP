#include "engine.h"

#include <cassert>

Engine *Engine::g_self{nullptr};

Engine::Engine() {
  assert(g_self == nullptr);
  g_self = this;
  m_memory.init(MB(800));
  m_io.init();
  m_audio.init();
  m_render.init();
  m_ui.init();
  m_phisics.init();
}

Engine::~Engine() {

  m_phisics.deinit();
  m_ui.deinit();
  m_render.deinit();
  m_audio.deinit();
  m_io.deinit();
  m_memory.deinit();
  g_self = nullptr;
}

void Engine::start() {

  while (true) {
  }
}
