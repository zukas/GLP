#include "io.h"
#include <cstdarg>

IO *IO::g_self{nullptr};

IO::IO() { g_self = this; }

IO::~IO() { g_self = nullptr; }

void IO::init() {}

void IO::deinit() {}

IO &IO::instance() { return *g_self; }

IO::log_item *IO::createEntry() { return m_alloc.alloc(); }

void IO::consumeEntry(IO::log_item *item) {}
