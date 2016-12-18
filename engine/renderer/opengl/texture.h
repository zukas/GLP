#ifndef TEXTURE_H
#define TEXTURE_H

#include <cstdint>

uint32_t load_texture(const char *file);
void destroy_texture(uint32_t texture_id);

#endif // TEXTURE_H
