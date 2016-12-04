#ifndef ENGINE_H
#define ENGINE_H

#ifdef __GNUG__
#define NORETURN __attribute__((noreturn))
#endif

class Engine {
private:
  struct EngineInternal *m_internal;

public:
  Engine();
  ~Engine();
  NORETURN void run();
};

#endif // ENGINE_H
