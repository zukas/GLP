#ifndef SWAP_CHAIN_H
#define SWAP_CHAIN_H

#include "engine_info.h"

bool vk_swap_chain_init();
void vk_swap_chain_deinit();

VkSwapchainKHR vk_swap_chain_get();
VkFormat vk_swap_chain_image_format_get();
VkExtent2D vk_swap_chain_extent_get();

#endif // SWAP_CHAIN_H
