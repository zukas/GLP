#include "io.h"

IO *IO::g_self{nullptr};

IO::IO() { g_self = this; }

IO::~IO() {}

void IO::init() {}

void IO::deinit() {}

IO &IO::instance() { return *g_self; }

void IO::write(log_item item) {

  switch (item.level) {
  case log_level::LOG: {
    printf("[LOG] %s\n", item.buffer);
    break;
  }
  case log_level::INFO: {
    printf("[INFO] %s\n", item.buffer);
    break;
  }
  case log_level::WARNING: {
    printf("[WARNING] %s\n", item.buffer);
    break;
  }
  case log_level::ERROR: {
    printf("[ERROR] %s\n", item.buffer);
    break;
  }
  }
}
