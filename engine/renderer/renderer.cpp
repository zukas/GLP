#include "renderer.h"

#include "vk/frame_buffers.h"
#include "vk/graphics_pipeline.h"
#include "vk/image_views.h"
#include "vk/instance.h"
#include "vk/logical_device.h"
#include "vk/phisical_device.h"
#include "vk/queue.h"
#include "vk/swap_chain.h"
#include "vk/window.h"

#include <cstring>

// Interface functions

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
  return status;
}

void renderer_deinit() {

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
