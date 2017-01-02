#include "io.h"

#include "io_memory.h"

#include <cstdarg>
#include <cstdio>
#include <cstring>

bool io_init() { return io_buffers_init(); }

void io_deinit() { io_buffers_deinit(); }

#define PTRINT_BASE(header, format)                                            \
  static const char *const cap = header;                                       \
  static const size_t cap_len = strlen(cap);                                   \
  char *buff = io_buffers_get();                                               \
  memcpy(buff, cap, cap_len);                                                  \
  va_list argptr;                                                              \
  va_start(argptr, format);                                                    \
  int written =                                                                \
      vsnprintf(&buff[cap_len], IO_BUFFER_SIZE - cap_len - 1, format, argptr); \
  va_end(argptr);                                                              \
  memcpy(&buff[cap_len + static_cast<size_t>(written)], "\n", 1);              \
  fwrite(buff, 1, cap_len + static_cast<size_t>(written) + 1, stdout);         \
  io_buffer_release(buff)

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
