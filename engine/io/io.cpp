#include "io.h"

#include "io_memory.h"

#include <cstdarg>
#include <cstdio>
#include <cstring>

IO::IO() {}

IO::~IO() {}

bool IO::init() {
  io_mem_init();
  return true;
}

void IO::deinit() { io_mem_deinit(); }

#define PTRINT_BASE(header, format)                                            \
  static const char *const cap = header;                                       \
  static const size_t cap_len = strlen(cap);                                   \
  static const size_t max_len = io_mem_buffer_size();                          \
  char *buff = io_buffer_alloc();                                              \
  memcpy(buff, cap, cap_len);                                                  \
  va_list argptr;                                                              \
  va_start(argptr, format);                                                    \
  int written =                                                                \
      vsnprintf(&buff[cap_len], max_len - cap_len - 1, format, argptr);        \
  va_end(argptr);                                                              \
  memcpy(&buff[cap_len + static_cast<size_t>(written)], "\n", 1);              \
  fwrite(buff, 1, cap_len + static_cast<size_t>(written) + 1, stdout);         \
  io_buffer_free(buff)

void log(const char *__restrict format, ...) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
  PTRINT_BASE("[LOG] ", format);
#pragma GCC diagnostic pop
}

void error(const char *__restrict format, ...) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
  PTRINT_BASE("[ERROR] ", format);
#pragma GCC diagnostic pop
}
