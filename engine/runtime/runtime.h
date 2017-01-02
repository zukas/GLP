#ifndef RUNTIME_H
#define RUNTIME_H

#include "engine_info.h"

bool runtime_init(const engine_description &desc);
void runtime_deinit();

#endif // RUNTIME_H
