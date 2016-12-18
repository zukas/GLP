#ifndef IO_H
#define IO_H

#include "utils/global_instance.h"
#include "utils/sysdef.h"

class IO : public global_instance<IO> {

public:
  IO();
  ~IO();
  bool init();
  void deinit();
};

using G_IO = global_instance<IO>;

void log(const char *__restrict format, ...);
void error(const char *__restrict format, ...);
#endif // IO_H
