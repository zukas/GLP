#include "engine_include.h"
#include "memory/global_malloc.h"
#include "renderer/vk/commands.h"
#include "renderer/vk/frame_buffers.h"
#include "renderer/vk/graphics_pipeline.h"
#include "renderer/vk/logical_device.h"

#include <cstdio>

struct file_info {
  char *data;
  size_t size;
};

file_info load_file(const char *name) {
  FILE *file = fopen(name, "rb");
  if (file) {

    char *buffer = static_cast<char *>(glp_malloc(16 * 1024));
    size_t size = fread(buffer, 1, 16 * 1024, file);
    fclose(file);
    return {buffer, size};
  }
  return {nullptr, 0};
}

void release_file(file_info info) {
  if (info.data) {
    glp_free(info.data);
  }
}

int main() {
  engine_description desc;

  desc.app = {"Testing Vulkan", 1, 0, 0};

  if (runtime_init(desc)) {

    file_info vert = load_file("shaders/demo_vert.spv");
    file_info frag = load_file("shaders/demo_frag.spv");

    auto vert_mod = vk_shader_create(vert.data, vert.size);
    auto frag_mod = vk_shader_create(frag.data, frag.size);

    release_file(vert);
    release_file(frag);

    pipline_description pipe_desc{};
    pipe_desc.shaders[0].module = vert_mod;
    pipe_desc.shaders[0].type = VK_SHADER_STAGE_VERTEX_BIT;
    pipe_desc.shaders[1].module = frag_mod;
    pipe_desc.shaders[1].type = VK_SHADER_STAGE_FRAGMENT_BIT;

    pipe_desc.size = 2;

    VkPipelineExt pipeline;
    vk_pipeline_create(pipe_desc, &pipeline);

    VkFramebuffersExt frame_buffers;
    vk_frame_buffers_create(pipe_desc, pipeline, &frame_buffers);

    VkCommandBuffersExt cmd_buffers;
    vk_cmd_buffers_create(frame_buffers, &cmd_buffers);

    vk_cmd_buffers_record(pipeline, frame_buffers, cmd_buffers);

    for (int i = 0; i < 1000; ++i) {
      renderer_execute(cmd_buffers);
    }

    vkDeviceWaitIdle(vk_lg_device_get());
    vk_cmd_buffers_destroy(cmd_buffers);
    vk_frame_buffers_destroy(frame_buffers);
    vk_pipline_destroy(pipeline);
    vk_shader_destroy(vert_mod);
    vk_shader_destroy(frag_mod);

  } else {
    printf("Somthing when wrong\n");
  }
  runtime_deinit();
}
