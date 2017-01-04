#include "renderer.h"

#include "vk/commands.h"
#include "vk/frame_buffers.h"
#include "vk/graphics_pipeline.h"
#include "vk/image_views.h"
#include "vk/instance.h"
#include "vk/internal/types.h"
#include "vk/logical_device.h"
#include "vk/phisical_device.h"
#include "vk/queue.h"
#include "vk/swap_chain.h"
#include "vk/window.h"

#include <cstring>

struct {
  VkQueue graphics_queue;
  VkQueue present_queue;
  VkSemaphore image_ready;
  VkSemaphore frame_done;
} __context;

void demo_init() {

  __context.graphics_queue = vk_queue_graphics_get();
  __context.present_queue = vk_queue_present_get();

  VkSemaphoreCreateInfo semaphoreInfo = {};
  semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
  vkCreateSemaphore(vk_lg_device_get(), &semaphoreInfo, nullptr,
                    &__context.image_ready);
  vkCreateSemaphore(vk_lg_device_get(), &semaphoreInfo, nullptr,
                    &__context.frame_done);
}

void demo_deinit() {

  vkDestroySemaphore(vk_lg_device_get(), __context.image_ready, nullptr);
  vkDestroySemaphore(vk_lg_device_get(), __context.frame_done, nullptr);
  __context.graphics_queue = VK_NULL_HANDLE;
  __context.present_queue = VK_NULL_HANDLE;
  __context.image_ready = VK_NULL_HANDLE;
  __context.frame_done = VK_NULL_HANDLE;
}

bool renderer_init(const engine_description &desc) {

  bool status = vk_window_init(desc.app.app_name);
  status = status && vk_instance_init(desc.app);
  status = status && vk_window_surface_init();
  status = status && vk_ph_device_init(desc.device);
  status = status && vk_lg_device_init();
  status = status && vk_queue_init();
  status = status && vk_swap_chain_init();
  status = status && vk_image_views_init();
  status = status && vk_frame_buffers_init(1024);
  status = status && vk_pipeline_init(1024);
  status = status && vk_cmd_pool_init(1024);

  demo_init();

  return status;
}

void renderer_deinit() {

  vkDeviceWaitIdle(vk_lg_device_get());

  demo_deinit();

  vk_cmd_pool_deinit();
  vk_pipeline_deinit();
  vk_frame_buffers_deinit();
  vk_image_views_deinit();
  vk_swap_chain_deinit();
  vk_queue_deinit();
  vk_lg_device_deinit();
  vk_ph_device_deinit();
  vk_window_surface_deinit();
  vk_instance_deinit();
  vk_window_deinit();
}

void renderer_execute(const VkCommandBuffersExt cmd_buffers) {

  uint32_t imageIndex;
  vkAcquireNextImageKHR(vk_lg_device_get(), vk_swap_chain_get(),
                        std::numeric_limits<uint64_t>::max(),
                        __context.image_ready, VK_NULL_HANDLE, &imageIndex);

  VkSubmitInfo submitInfo = {};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

  VkSemaphore waitSemaphores[] = {__context.image_ready};
  VkPipelineStageFlags waitStages[] = {
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  submitInfo.waitSemaphoreCount = 1;
  submitInfo.pWaitSemaphores = waitSemaphores;
  submitInfo.pWaitDstStageMask = waitStages;

  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &cmd_buffers->command_buffers[imageIndex];

  VkSemaphore signalSemaphores[] = {__context.frame_done};
  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores = signalSemaphores;

  vkQueueSubmit(__context.graphics_queue, 1, &submitInfo, VK_NULL_HANDLE);

  VkPresentInfoKHR presentInfo = {};
  presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores = signalSemaphores;

  VkSwapchainKHR swapChains[] = {vk_swap_chain_get()};
  presentInfo.swapchainCount = 1;
  presentInfo.pSwapchains = swapChains;

  presentInfo.pImageIndices = &imageIndex;

  vkQueuePresentKHR(__context.present_queue, &presentInfo);
}
