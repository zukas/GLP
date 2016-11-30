#ifndef IO_H
#define IO_H

#include <atomic>
#include <cstdio>

#include "memory/atomic_pool_allocator.h"

class IO {
public:
  enum class log_level : char { ERROR, WARNING, INFO, LOG };
  struct log_item {
    log_level level;
    char buffer[255];
  } __attribute__((packed));

public:
  IO();
  ~IO();
  void init();
  void deinit();
  static IO &instance();

public:
  log_item *createEntry();
  void consumeEntry(log_item *item);

private:
  static IO *g_self;
  std::atomic_long m_reader;
  std::atomic_long m_idx[2];
  log_item *m_backlog[2][256];
  atomic_pool_allocator<log_item, 8192> m_alloc;
};

template <typename... __Args>
void write(IO::log_level level, const char *__restrict format, __Args... args) {
  auto &io = IO::instance();
  auto entry = io.createEntry();
  snprintf(entry->buffer, 255, format, args...);
  entry->level = level;
  io.consumeEntry(entry);
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
