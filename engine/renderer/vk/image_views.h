#ifndef IMAGE_VIEWS_H
#define IMAGE_VIEWS_H

#include "engine_info.h"

bool vk_image_views_init();
void vk_image_views_deinit();

VkImageView *vk_image_views_get();
size_t vk_image_views_size();

#endif // IMAGE_VIEWS_H
