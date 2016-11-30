#ifndef ENGINE_H
#define ENGINE_H

#include "audio/audio.h"
#include "interface/interface.h"
#include "io/io.h"
#include "memory/memory.h"
#include "phisics/phisics.h"
#include "render/render.h"
#include "resource/resource.h"

class Engine {
private:
  static Engine *g_self;
  Memory m_memory;
  IO m_io;
  Resource m_resource;
  Audio m_audio;
  Phisics m_phisics;
  Render m_render;
  Interface m_ui;

public:
  Engine();
  ~Engine();
  __attribute__((noreturn)) void start();
};

#endif // ENGINE_H
