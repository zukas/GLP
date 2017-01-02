#ifndef VALIDATION_H
#define VALIDATION_H

#include "engine_info.h"

#ifdef NDEBUG
constexpr const char *validation_layers[]{};
constexpr size_t validation_layer_size{0};

#else
constexpr const char *validation_layers[] = {
    "VK_LAYER_LUNARG_standard_validation"};
constexpr uint32_t validation_layer_size{sizeof(validation_layers) /
                                         sizeof(const char *)};
#endif

constexpr const char *device_extensions[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
constexpr uint32_t device_extension_size{sizeof(device_extensions) /
                                         sizeof(const char *)};

#endif // VALIDATION_H
