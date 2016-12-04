#ifndef IO_H
#define IO_H

#include "utils/sysdef.h"

#include <cstdio>

class IO {
public:
  enum class log_level : char { ERROR, WARNING, INFO, LOG };
  struct log_item {
    log_level level;
    char buffer[255];
  } PACKED;

public:
  IO();
  ~IO();
  void init();
  void deinit();
  static IO &instance();

public:
  void write(log_item item);

private:
  static IO *g_self;
};

template <typename... __Args>
void write(IO::log_level level, const char *__restrict format, __Args... args) {
  auto &io = IO::instance();
  IO::log_item entry;
  entry.level = level;
  snprintf(entry.buffer, 255, format, args...);
  io.write(entry);
}

template <typename... __Args>
void log(const char *__restrict format, __Args... args) {
  write(IO::log_level::LOG, format, args...);
}

template <typename... __Args>
void info(const char *__restrict format, __Args... args) {
  write(IO::log_level::INFO, format, args...);
}

template <typename... __Args>
void warning(const char *__restrict format, __Args... args) {
  write(IO::log_level::WARNING, format, args...);
}

template <typename... __Args>
void error(const char *__restrict format, __Args... args) {
  write(IO::log_level::ERROR, format, args...);
}

#endif // IO_H
