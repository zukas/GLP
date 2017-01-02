#include "graphics_pipeline.h"

#include "internal/types.h"
#include "logical_device.h"
#include "memory/pool_allocator.h"
#include "memory/type_allocator.h"
#include "swap_chain.h"

using allocator = type_allocator<VkPipelineExt_T, pool_allocator>;

struct {
  allocator pipeline_allocator;
} __context;

bool vk_pipeline_init(size_t count) {
  return __context.pipeline_allocator.init(sizeof(VkPipelineExt_T), count);
}

void vk_pipeline_deinit() { __context.pipeline_allocator.deinit(); }

VkShaderModule vk_shader_create(const char *source, size_t length) {
  VkShaderModuleCreateInfo module_info = {};
  module_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  module_info.codeSize = length;
  module_info.pCode = reinterpret_cast<const uint32_t *>(source);
  VkShaderModule modele{VK_NULL_HANDLE};
  VkResult res =
      vkCreateShaderModule(vk_lg_device_get(), &module_info, nullptr, &modele);
  return res == VK_SUCCESS ? modele : VK_NULL_HANDLE;
}

void vk_shader_destroy(VkShaderModule module) {
  vkDestroyShaderModule(vk_lg_device_get(), module, nullptr);
}

void vk_render_pass_destroy(VkRenderPass render_pass) {
  vkDestroyRenderPass(vk_lg_device_get(), render_pass, nullptr);
}

bool vk_pipeline_create(const pipline_description &desc,
                        VkPipelineExt *pipeline) {

  VkResult res;
  VkDevice device = vk_lg_device_get();
  // Describing pipline layout
  VkPipelineLayoutCreateInfo layout_info = {};
  layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  layout_info.setLayoutCount = 0;         // Optional
  layout_info.pSetLayouts = nullptr;      // Optional
  layout_info.pushConstantRangeCount = 0; // Optional
  layout_info.pPushConstantRanges = 0;    // Optional

  VkPipelineLayout layout;
  res = vkCreatePipelineLayout(device, &layout_info, nullptr, &layout);
  if (res != VK_SUCCESS) {
    vkDestroyPipelineLayout(device, layout, nullptr);
    return false;
  }

  // Describing render pass stage
  VkAttachmentDescription color_att = {};
  color_att.format = vk_swap_chain_image_format_get();
  color_att.samples = VK_SAMPLE_COUNT_1_BIT;
  color_att.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  color_att.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

  color_att.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  color_att.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

  color_att.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  color_att.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  VkAttachmentReference color_att_ref = {};
  color_att_ref.attachment = 0;
  color_att_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkSubpassDescription subpass = {};
  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.colorAttachmentCount = 1;
  subpass.pColorAttachments = &color_att_ref;

  VkRenderPassCreateInfo render_pass_info = {};
  render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  render_pass_info.attachmentCount = 1;
  render_pass_info.pAttachments = &color_att;
  render_pass_info.subpassCount = 1;
  render_pass_info.pSubpasses = &subpass;

  VkRenderPass render_pass;
  res = vkCreateRenderPass(device, &render_pass_info, nullptr, &render_pass);
  if (res != VK_SUCCESS) {
    vkDestroyRenderPass(device, render_pass, nullptr);
    vkDestroyPipelineLayout(device, layout, nullptr);
    return false;
  }

  // Describing pipline
  VkPipelineShaderStageCreateInfo stage_info[4]{};
  for (uint32_t i = 0; i < desc.size; i++) {
    stage_info[i].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    stage_info[i].stage = desc.shaders[i].type;
    stage_info[i].module = desc.shaders[i].module;
    stage_info[i].pName = "main";
  }

  VkPipelineVertexInputStateCreateInfo vertex_info = {};
  vertex_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertex_info.vertexBindingDescriptionCount = 0;
  vertex_info.pVertexBindingDescriptions = nullptr; // Optional
  vertex_info.vertexAttributeDescriptionCount = 0;
  vertex_info.pVertexAttributeDescriptions = nullptr; // Optional

  VkPipelineInputAssemblyStateCreateInfo input_asm = {};
  input_asm.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  input_asm.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  input_asm.primitiveRestartEnable = VK_FALSE;

  VkExtent2D extent = vk_swap_chain_extent_get();
  VkViewport viewport = {};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = static_cast<float>(extent.width);
  viewport.height = static_cast<float>(extent.height);
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;

  VkRect2D scissor = {};
  scissor.offset = {0, 0};
  scissor.extent = extent;

  VkPipelineViewportStateCreateInfo viewport_state = {};
  viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewport_state.viewportCount = 1;
  viewport_state.pViewports = &viewport;
  viewport_state.scissorCount = 1;
  viewport_state.pScissors = &scissor;

  VkPipelineRasterizationStateCreateInfo rasterizer = {};
  rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterizer.depthClampEnable = VK_FALSE;
  rasterizer.rasterizerDiscardEnable = VK_FALSE;
  rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
  rasterizer.lineWidth = 1.0f;
  rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
  rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
  rasterizer.depthBiasEnable = VK_FALSE;
  rasterizer.depthBiasConstantFactor = 0.0f; // Optional
  rasterizer.depthBiasClamp = 0.0f;          // Optional
  rasterizer.depthBiasSlopeFactor = 0.0f;    // Optional

  VkPipelineMultisampleStateCreateInfo multisampling = {};
  multisampling.sType =
      VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisampling.sampleShadingEnable = VK_FALSE;
  multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
  multisampling.minSampleShading = 1.0f;          // Optional
  multisampling.pSampleMask = nullptr;            /// Optional
  multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
  multisampling.alphaToOneEnable = VK_FALSE;      // Optional

  VkPipelineColorBlendAttachmentState color_blent_attach = {};
  color_blent_attach.colorWriteMask =
      VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
      VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  color_blent_attach.blendEnable = VK_FALSE;
  color_blent_attach.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;  // Optional
  color_blent_attach.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
  color_blent_attach.colorBlendOp = VK_BLEND_OP_ADD;             // Optional
  color_blent_attach.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;  // Optional
  color_blent_attach.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
  color_blent_attach.alphaBlendOp = VK_BLEND_OP_ADD;             // Optional

  VkPipelineColorBlendStateCreateInfo color_blending = {};
  color_blending.sType =
      VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  color_blending.logicOpEnable = VK_FALSE;
  color_blending.logicOp = VK_LOGIC_OP_COPY; // Optional
  color_blending.attachmentCount = 1;
  color_blending.pAttachments = &color_blent_attach;
  color_blending.blendConstants[0] = 0.0f; // Optional
  color_blending.blendConstants[1] = 0.0f; // Optional
  color_blending.blendConstants[2] = 0.0f; // Optional
  color_blending.blendConstants[3] = 0.0f; // Optional

  VkDynamicState dynamic_states[] = {VK_DYNAMIC_STATE_VIEWPORT,
                                     VK_DYNAMIC_STATE_LINE_WIDTH};

  VkPipelineDynamicStateCreateInfo dynamic_state_info = {};
  dynamic_state_info.sType =
      VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  dynamic_state_info.dynamicStateCount = 2;
  dynamic_state_info.pDynamicStates = dynamic_states;

  VkGraphicsPipelineCreateInfo pipeline_info = {};
  pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipeline_info.stageCount = desc.size;
  pipeline_info.pStages = stage_info;

  pipeline_info.pVertexInputState = &vertex_info;
  pipeline_info.pInputAssemblyState = &input_asm;
  pipeline_info.pViewportState = &viewport_state;
  pipeline_info.pRasterizationState = &rasterizer;
  pipeline_info.pMultisampleState = &multisampling;
  pipeline_info.pDepthStencilState = nullptr; // Optional
  pipeline_info.pColorBlendState = &color_blending;
  pipeline_info.pDynamicState = &dynamic_state_info; // Optional

  pipeline_info.layout = layout;

  pipeline_info.renderPass = render_pass;
  pipeline_info.subpass = 0;
  pipeline_info.basePipelineHandle = VK_NULL_HANDLE;
  pipeline_info.basePipelineIndex = -1; // Optional

  VkPipeline pipeline_obj;
  res = vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipeline_info,
                                  nullptr, &pipeline_obj);

  if (res != VK_SUCCESS) {
    vkDestroyPipeline(device, pipeline_obj, nullptr);
    vkDestroyRenderPass(device, render_pass, nullptr);
    vkDestroyPipelineLayout(device, layout, nullptr);
  } else {
    VkPipelineExt_T *obj = __context.pipeline_allocator.make_new();
    obj->pipeline = pipeline_obj;
    obj->layout = layout;
    obj->render_pass = render_pass;
    *pipeline = obj;
  }

  return res == VK_SUCCESS;
}

void vk_pipline_destroy(const VkPipelineExt pipeline) {
  VkDevice device = vk_lg_device_get();
  if (pipeline) {
    vkDestroyPipeline(device, pipeline->pipeline, nullptr);
    vkDestroyRenderPass(device, pipeline->render_pass, nullptr);
    vkDestroyPipelineLayout(device, pipeline->layout, nullptr);
    __context.pipeline_allocator.destroy(pipeline);
  }
}
