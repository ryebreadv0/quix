#ifndef _QUIX_PIPELINE_CPP
#define _QUIX_PIPELINE_CPP

#include "quix_pipeline.hpp"

#include "quix_device.hpp"
#include "quix_instance.hpp"
#include "quix_shader.hpp"

namespace quix {

namespace graphics {

    namespace defaults {

        static constexpr VkPipelineLayoutCreateInfo layout_create_info = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .setLayoutCount = 0,
            .pSetLayouts = nullptr,
            .pushConstantRangeCount = 0,
            .pPushConstantRanges = nullptr
        };

    } // namespace defaults

    VkPipelineShaderStageCreateInfo pipeline_builder::create_shader_stage(
        const char* file_path, const VkShaderStageFlagBits shader_stage)
    {
        EShLanguage EShStage;
        switch (shader_stage) {
        case VK_SHADER_STAGE_VERTEX_BIT:
            EShStage = EShLangVertex;
            break;
        case VK_SHADER_STAGE_FRAGMENT_BIT:
            EShStage = EShLangFragment;
            break;
        case VK_SHADER_STAGE_GEOMETRY_BIT:
            EShStage = EShLangGeometry;
            break;
        case VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT:
            EShStage = EShLangTessControl;
            break;
        case VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT:
            EShStage = EShLangTessEvaluation;
            break;
        case VK_SHADER_STAGE_COMPUTE_BIT:
            EShStage = EShLangCompute;
            break;
        default:
            throw std::runtime_error("invalid shader stage!");
        }

        shader shader_obj(file_path, EShStage);
        VkShaderModule shader_module = shader_obj.createShaderModule(device);

        return VkPipelineShaderStageCreateInfo {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .stage = shader_stage,
            .module = shader_module,
            .pName = "main",
            .pSpecializationInfo = nullptr
        };
    }




    // pipeline_info struct

    // pipeline_info struct end

    void pipeline::create_pipeline_layout(VkDevice device, const VkPipelineLayoutCreateInfo* pipeline_create_info)
    {

        if (pipeline_create_info == nullptr) {
            pipeline_create_info = &defaults::layout_create_info;
        }

        if (vkCreatePipelineLayout(device, pipeline_create_info, nullptr, &pipeline_layout) != VK_SUCCESS) {
            spdlog::error("failed to create pipeline layout!");
            throw std::runtime_error("failed to create pipeline layout!");
        }
    }

    void pipeline::create_renderpass(VkDevice device, const renderpass_info* renderpass_info)
    {
        VkRenderPassCreateInfo renderpass_create_info = {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .attachmentCount = (uint32_t)renderpass_info->attachments.size(),
            .pAttachments = renderpass_info->attachments.data(),
            .subpassCount = (uint32_t)renderpass_info->subpasses.size(),
            .pSubpasses = renderpass_info->subpasses.data(),
            .dependencyCount = (uint32_t)renderpass_info->subpass_dependencies.size(),
            .pDependencies = renderpass_info->subpass_dependencies.data()
        };

        if (vkCreateRenderPass(device, &renderpass_create_info, nullptr, &render_pass) != VK_SUCCESS) {
            spdlog::error("failed to create render pass!");
            throw std::runtime_error("failed to create render pass!");
        }
    }

    void pipeline::create_pipeline(VkDevice device, const VkGraphicsPipelineCreateInfo* pipeline_create_info)
    {
        // VkGraphicsPipelineCreateInfo pipeline_create_info = {
        //     .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        //     .pNext = nullptr,
        //     .flags = 0,
        //     .stageCount = pipeline_info->shader_stage_count,
        //     .pStages = pipeline_info->shader_stages,
        //     .pVertexInputState = &pipeline_info->vertex_input_state,
        //     .pInputAssemblyState = &pipeline_info->input_assembly_state,
        //     .pTessellationState = &pipeline_info->tessellation_state,
        //     .pViewportState = &pipeline_info->viewport_state,
        //     .pRasterizationState = &pipeline_info->rasterization_state,
        //     .pMultisampleState = &pipeline_info->multisample_state,
        //     .pDepthStencilState = &pipeline_info->depth_stencil_state,
        //     .pColorBlendState = &pipeline_info->color_blend_state,
        //     .pDynamicState = &pipeline_info->dynamic_state,
        //     .layout = pipeline_layout,
        //     .renderPass = render_pass,
        //     .subpass = 0,
        //     .basePipelineHandle = VK_NULL_HANDLE,
        //     .basePipelineIndex = -1
        // };

        // pipeline_info->graphics_pipeline_create_info.layout = pipeline_layout;
        // pipeline_info->graphics_pipeline_create_info.renderPass = render_pass;

        // if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipeline_info->graphics_pipeline_create_info, nullptr, &pipeline) != VK_SUCCESS) {
        //     spdlog::error("failed to create graphics pipeline!");
        //     throw std::runtime_error("failed to create graphics pipeline!");
        // }
    }

} // namespace graphics

} // namespace quix

#endif // _QUIX_PIPELINE_CPP

// namespace defaults {

//     static constexpr VkPipelineVertexInputStateCreateInfo vertex_input_state_create_info = {
//         .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
//         .pNext = nullptr,
//         .flags = 0,
//         .vertexBindingDescriptionCount = 0,
//         .pVertexBindingDescriptions = nullptr,
//         .vertexAttributeDescriptionCount = 0,
//         .pVertexAttributeDescriptions = nullptr
//     };

//     static constexpr VkPipelineInputAssemblyStateCreateInfo input_assembly_state_create_info_triangle = {
//         .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
//         .pNext = nullptr,
//         .flags = 0,
//         .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
//         .primitiveRestartEnable = VK_FALSE
//     };

//     static constexpr VkPipelineInputAssemblyStateCreateInfo input_assembly_state_create_info_patch = {
//         .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
//         .pNext = nullptr,
//         .flags = 0,
//         .topology = VK_PRIMITIVE_TOPOLOGY_PATCH_LIST,
//         .primitiveRestartEnable = VK_FALSE
//     };

//     static constexpr VkPipelineInputAssemblyStateCreateInfo input_assembly_state_create_info_point = {
//         .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
//         .pNext = nullptr,
//         .flags = 0,
//         .topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST,
//         .primitiveRestartEnable = VK_FALSE
//     };

//     static constexpr std::array<VkDynamicState, 2> dynamic_states = {
//         VK_DYNAMIC_STATE_VIEWPORT,
//         VK_DYNAMIC_STATE_SCISSOR
//     };

//     static constexpr VkPipelineDynamicStateCreateInfo dynamic_state_create_info = {
//         .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
//         .pNext = nullptr,
//         .flags = 0,
//         .dynamicStateCount = dynamic_states.size(),
//         .pDynamicStates = dynamic_states.data()
//     };

//     static constexpr VkPipelineViewportStateCreateInfo viewport_state_create_info = {
//         .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
//         .pNext = nullptr,
//         .flags = 0,
//         .viewportCount = 1,
//         .pViewports = nullptr,
//         .scissorCount = 1,
//         .pScissors = nullptr
//     };

//     static constexpr VkPipelineRasterizationStateCreateInfo rasterization_state_info_back_cw = {
//         .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
//         .pNext = nullptr,
//         .flags = 0,
//         .depthClampEnable = VK_FALSE,
//         .rasterizerDiscardEnable = VK_FALSE,
//         .polygonMode = VK_POLYGON_MODE_FILL,
//         .cullMode = VK_CULL_MODE_BACK_BIT,
//         .frontFace = VK_FRONT_FACE_CLOCKWISE,
//         .depthBiasEnable = VK_FALSE,
//         .depthBiasConstantFactor = 0.0f,
//         .depthBiasClamp = 0.0f,
//         .depthBiasSlopeFactor = 0.0f,
//         .lineWidth = 1.0f
//     };

//     static constexpr VkPipelineRasterizationStateCreateInfo rasterization_state_info_front_cw = {
//         .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
//         .pNext = nullptr,
//         .flags = 0,
//         .depthClampEnable = VK_FALSE,
//         .rasterizerDiscardEnable = VK_FALSE,
//         .polygonMode = VK_POLYGON_MODE_FILL,
//         .cullMode = VK_CULL_MODE_FRONT_BIT,
//         .frontFace = VK_FRONT_FACE_CLOCKWISE,
//         .depthBiasEnable = VK_FALSE,
//         .depthBiasConstantFactor = 0.0f,
//         .depthBiasClamp = 0.0f,
//         .depthBiasSlopeFactor = 0.0f,
//         .lineWidth = 1.0f
//     };

//     static constexpr VkPipelineRasterizationStateCreateInfo rasterization_state_info_back_ccw = {
//         .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
//         .pNext = nullptr,
//         .flags = 0,
//         .depthClampEnable = VK_FALSE,
//         .rasterizerDiscardEnable = VK_FALSE,
//         .polygonMode = VK_POLYGON_MODE_FILL,
//         .cullMode = VK_CULL_MODE_BACK_BIT,
//         .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
//         .depthBiasEnable = VK_FALSE,
//         .depthBiasConstantFactor = 0.0f,
//         .depthBiasClamp = 0.0f,
//         .depthBiasSlopeFactor = 0.0f,
//         .lineWidth = 1.0f
//     };

//     static constexpr VkPipelineRasterizationStateCreateInfo rasterization_state_info_front_ccw = {
//         .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
//         .pNext = nullptr,
//         .flags = 0,
//         .depthClampEnable = VK_FALSE,
//         .rasterizerDiscardEnable = VK_FALSE,
//         .polygonMode = VK_POLYGON_MODE_FILL,
//         .cullMode = VK_CULL_MODE_FRONT_BIT,
//         .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
//         .depthBiasEnable = VK_FALSE,
//         .depthBiasConstantFactor = 0.0f,
//         .depthBiasClamp = 0.0f,
//         .depthBiasSlopeFactor = 0.0f,
//         .lineWidth = 1.0f
//     };

//     static constexpr VkPipelineDepthStencilStateCreateInfo depth_stencil_state_create_info_off = {
//         .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
//         .pNext = nullptr,
//         .flags = 0,
//         .depthTestEnable = VK_FALSE,
//         .depthWriteEnable = VK_FALSE,
//         .depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL,
//         .depthBoundsTestEnable = VK_FALSE,
//         .stencilTestEnable = VK_FALSE,
//         .front = {},
//         .back = {},
//         .minDepthBounds = 0.0f,
//         .maxDepthBounds = 1.0f
//     };

//     static constexpr VkPipelineDepthStencilStateCreateInfo depth_stencil_state_create_info_on = {
//         .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
//         .pNext = nullptr,
//         .flags = 0,
//         .depthTestEnable = VK_TRUE,
//         .depthWriteEnable = VK_TRUE,
//         .depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL,
//         .depthBoundsTestEnable = VK_FALSE,
//         .stencilTestEnable = VK_FALSE,
//         .front = {},
//         .back = {},
//         .minDepthBounds = 0.0f,
//         .maxDepthBounds = 1.0f
//     };

//     static constexpr VkPipelineMultisampleStateCreateInfo multisample_state_create_info = {
//         .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
//         .pNext = nullptr,
//         .flags = 0,
//         .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
//         .sampleShadingEnable = VK_FALSE,
//         .minSampleShading = 0.0f,
//         .pSampleMask = nullptr,
//         .alphaToCoverageEnable = VK_FALSE,
//         .alphaToOneEnable = VK_FALSE
//     };

//     // per framebuffer struct
//     static constexpr VkPipelineColorBlendAttachmentState color_blend_attachment_state_off = {
//         .blendEnable = VK_FALSE,
//         .srcColorBlendFactor = VK_BLEND_FACTOR_ONE,
//         .dstColorBlendFactor = VK_BLEND_FACTOR_ZERO,
//         .colorBlendOp = VK_BLEND_OP_ADD,
//         .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
//         .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
//         .alphaBlendOp = VK_BLEND_OP_ADD,
//         .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
//     };

//     static constexpr VkPipelineColorBlendAttachmentState color_blend_attachment_state_on = {
//         .blendEnable = VK_TRUE,
//         .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
//         .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
//         .colorBlendOp = VK_BLEND_OP_ADD,
//         .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
//         .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
//         .alphaBlendOp = VK_BLEND_OP_ADD,
//         .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
//     };

//     // global colorblending struct

//     static constexpr VkPipelineColorBlendStateCreateInfo color_blend_state_create_info_off = {
//         .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
//         .pNext = nullptr,
//         .flags = 0,
//         .logicOpEnable = VK_FALSE,
//         .logicOp = VK_LOGIC_OP_COPY, // optional
//         .attachmentCount = 1,
//         .pAttachments = nullptr,
//         .blendConstants = { 0.0f, 0.0f, 0.0f, 0.0f }
//     };

//     static constexpr VkPipelineColorBlendStateCreateInfo color_blend_state_create_info_on = {
//         .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
//         .pNext = nullptr,
//         .flags = 0,
//         .logicOpEnable = VK_FALSE,
//         .logicOp = VK_LOGIC_OP_COPY, // optional
//         .attachmentCount = 1,
//         .pAttachments = nullptr,
//         .blendConstants = { 0.0f, 0.0f, 0.0f, 0.0f }
//     };

//     static constexpr VkPipelineColorBlendStateCreateInfo color_blend_state_create_info_bitwise_overwrite = {
//         .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
//         .pNext = nullptr,
//         .flags = 0,
//         .logicOpEnable = VK_TRUE,
//         .logicOp = VK_LOGIC_OP_COPY,
//         .attachmentCount = 1,
//         .pAttachments = &color_blend_attachment_state_off,
//         .blendConstants = { 0.0f, 0.0f, 0.0f, 0.0f }
//     };

//     static constexpr VkPipelineTessellationStateCreateInfo tessellation_state_create_info = {
//         .sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO,
//         .pNext = nullptr,
//         .flags = 0,
//         .patchControlPoints = 3
//     };

// } // namespace defaults

// struct pipeline_info {
// private:
//     std::pmr::monotonic_buffer_resource memory_resource;
// public:
//     VkGraphicsPipelineCreateInfo graphics_pipeline_create_info{};

//     void init_defaults();

//     // Set the shader count, and allocate space
//     void allocate_shader_stages(uint32_t stage_count);
//     // Create the shader modules for each of the allocated shaders
//     void setup_shader_stage(VkDevice device, uint32_t index, const char* file_path, VkShaderStageFlagBits shader_stage);

//     MAYBEUNUSED VkPipelineVertexInputStateCreateInfo* get_vertex_input_state();
//     MAYBEUNUSED VkPipelineInputAssemblyStateCreateInfo* get_input_assembly_state();

//     MAYBEUNUSED VkPipelineViewportStateCreateInfo* get_viewport_state();
//     MAYBEUNUSED VkPipelineDynamicStateCreateInfo* get_dynamic_state();

//     MAYBEUNUSED VkPipelineRasterizationStateCreateInfo* get_rasterization_state();

//     MAYBEUNUSED VkPipelineColorBlendAttachmentState* get_color_blend_attachment_state();
//     MAYBEUNUSED VkPipelineColorBlendStateCreateInfo* get_color_blend_state();

//     MAYBEUNUSED VkPipelineDepthStencilStateCreateInfo* get_depth_stencil_state();
//     MAYBEUNUSED VkPipelineMultisampleStateCreateInfo* get_multisample_state();

//     MAYBEUNUSED VkPipelineTessellationStateCreateInfo* get_tessellation_state();

// };

// void pipeline_info::init_defaults()
// {
//     graphics_pipeline_create_info.basePipelineHandle = VK_NULL_HANDLE;
//     graphics_pipeline_create_info.basePipelineIndex = -1;

//     graphics_pipeline_create_info.flags = 0;

//     get_vertex_input_state();
//     get_input_assembly_state();

//     get_viewport_state();
//     get_dynamic_state();

//     get_rasterization_state();

//     get_color_blend_attachment_state();
//     get_color_blend_state();

//     get_depth_stencil_state();
//     get_multisample_state();

// }

// void pipeline_info::allocate_shader_stages(uint32_t stage_count)
// {
//     if (graphics_pipeline_create_info.pStages != nullptr)
//     {
//         spdlog::warn("shader stage create info has already been created");
//         return;
//     }

//     auto* pipeline_stages = (VkPipelineShaderStageCreateInfo*)memory_resource.allocate(sizeof(VkPipelineShaderStageCreateInfo) * stage_count);
//     graphics_pipeline_create_info.pStages = pipeline_stages;
//     graphics_pipeline_create_info.stageCount = stage_count;
// }

// MAYBEUNUSED void pipeline_info::setup_shader_stage(VkDevice device, uint32_t index, const char* file_path, VkShaderStageFlagBits shader_stage)
// {
//     if (index >= graphics_pipeline_create_info.stageCount) {
//         spdlog::error("index out of range");
//         return;
//     }

//     EShLanguage EShStage;
//     switch (shader_stage) {
//         case VK_SHADER_STAGE_VERTEX_BIT:
//             EShStage = EShLangVertex;
//             break;
//         case VK_SHADER_STAGE_FRAGMENT_BIT:
//             EShStage = EShLangFragment;
//             break;
//         case VK_SHADER_STAGE_GEOMETRY_BIT:
//             EShStage = EShLangGeometry;
//             break;
//         case VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT:
//             EShStage = EShLangTessControl;
//             break;
//         case VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT:
//             EShStage = EShLangTessEvaluation;
//             break;
//         case VK_SHADER_STAGE_COMPUTE_BIT:
//             EShStage = EShLangCompute;
//             break;
//         default:
//             throw std::runtime_error("invalid shader stage!");
//     }

//     shader shader_obj(file_path,EShStage);
//     VkShaderModule shader_module = shader_obj.createShaderModule(device);

//     auto* shader_stage_create_info = (VkPipelineShaderStageCreateInfo*)graphics_pipeline_create_info.pStages + index;

//     *shader_stage_create_info = VkPipelineShaderStageCreateInfo {
//         .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
//         .pNext = nullptr,
//         .flags = 0,
//         .stage = shader_stage,
//         .module = shader_module,
//         .pName = "main",
//         .pSpecializationInfo = nullptr
//     };
// }

// MAYBEUNUSED VkPipelineVertexInputStateCreateInfo* pipeline_info::get_vertex_input_state()
// {
//     if (graphics_pipeline_create_info.pVertexInputState != nullptr)
//         return (VkPipelineVertexInputStateCreateInfo*)graphics_pipeline_create_info.pVertexInputState;

//     auto* pipeline_state = (VkPipelineVertexInputStateCreateInfo*)memory_resource.allocate(sizeof(VkPipelineVertexInputStateCreateInfo));
//     *pipeline_state = defaults::vertex_input_state_create_info;
//     graphics_pipeline_create_info.pVertexInputState = pipeline_state;

//     return pipeline_state;
// }

// MAYBEUNUSED VkPipelineInputAssemblyStateCreateInfo* pipeline_info::get_input_assembly_state()
// {
//     if (graphics_pipeline_create_info.pInputAssemblyState != nullptr)
//         return (VkPipelineInputAssemblyStateCreateInfo*)graphics_pipeline_create_info.pInputAssemblyState;

//     auto* input_assembly_state = (VkPipelineInputAssemblyStateCreateInfo*)memory_resource.allocate(sizeof(VkPipelineInputAssemblyStateCreateInfo));
//     *input_assembly_state = defaults::input_assembly_state_create_info_triangle;
//     graphics_pipeline_create_info.pInputAssemblyState = input_assembly_state;

//     return input_assembly_state;
// }

// MAYBEUNUSED VkPipelineViewportStateCreateInfo* pipeline_info::get_viewport_state()
// {
//     if (graphics_pipeline_create_info.pViewportState != nullptr)
//         return (VkPipelineViewportStateCreateInfo*)graphics_pipeline_create_info.pViewportState;

//     auto* viewport_state = (VkPipelineViewportStateCreateInfo*)memory_resource.allocate(sizeof(VkPipelineViewportStateCreateInfo));
//     *viewport_state = defaults::viewport_state_create_info;
//     graphics_pipeline_create_info.pViewportState = viewport_state;

//     return viewport_state;
// }

// MAYBEUNUSED VkPipelineDynamicStateCreateInfo* pipeline_info::get_dynamic_state()
// {
//     if (graphics_pipeline_create_info.pDynamicState != nullptr)
//         return (VkPipelineDynamicStateCreateInfo*)graphics_pipeline_create_info.pDynamicState;

//     auto* dynamic_state = (VkPipelineDynamicStateCreateInfo*)memory_resource.allocate(sizeof(VkPipelineDynamicStateCreateInfo));
//     *dynamic_state = defaults::dynamic_state_create_info;
//     graphics_pipeline_create_info.pDynamicState = dynamic_state;

//     return dynamic_state;
// }

// MAYBEUNUSED VkPipelineRasterizationStateCreateInfo* pipeline_info::get_rasterization_state()
// {
//     if (graphics_pipeline_create_info.pRasterizationState != nullptr)
//         return (VkPipelineRasterizationStateCreateInfo*)graphics_pipeline_create_info.pRasterizationState;

//     auto* rasterization_state = (VkPipelineRasterizationStateCreateInfo*)memory_resource.allocate(sizeof(VkPipelineRasterizationStateCreateInfo));
//     *rasterization_state = defaults::rasterization_state_info_back_cw;
//     graphics_pipeline_create_info.pRasterizationState = rasterization_state;

//     return rasterization_state;
// }

// MAYBEUNUSED VkPipelineColorBlendAttachmentState* pipeline_info::get_color_blend_attachment_state()
// {
//     if (graphics_pipeline_create_info.pColorBlendState != nullptr) {
//         if (graphics_pipeline_create_info.pColorBlendState->pAttachments != nullptr)
//             return (VkPipelineColorBlendAttachmentState*)graphics_pipeline_create_info.pColorBlendState->pAttachments;
//     } else {
//         get_color_blend_state();
//     }

//     auto* color_blend_attachment_state = (VkPipelineColorBlendAttachmentState*)memory_resource.allocate(sizeof(VkPipelineColorBlendAttachmentState));
//     *color_blend_attachment_state = defaults::color_blend_attachment_state_off;

//     auto* color_blend_state_info = (VkPipelineColorBlendStateCreateInfo*)graphics_pipeline_create_info.pColorBlendState;

//     color_blend_state_info->pAttachments = color_blend_attachment_state;

//     return color_blend_attachment_state;
// }

// MAYBEUNUSED VkPipelineColorBlendStateCreateInfo* pipeline_info::get_color_blend_state()
// {
//     if (graphics_pipeline_create_info.pColorBlendState != nullptr)
//         return (VkPipelineColorBlendStateCreateInfo*)graphics_pipeline_create_info.pColorBlendState;

//     auto* color_blend_state_info = (VkPipelineColorBlendStateCreateInfo*)memory_resource.allocate(sizeof(VkPipelineColorBlendStateCreateInfo));
//     *color_blend_state_info = defaults::color_blend_state_create_info_off;

//     graphics_pipeline_create_info.pColorBlendState = color_blend_state_info;

//     return color_blend_state_info;
// }

// MAYBEUNUSED VkPipelineDepthStencilStateCreateInfo* pipeline_info::get_depth_stencil_state()
// {
//     if (graphics_pipeline_create_info.pDepthStencilState != nullptr)
//         return (VkPipelineDepthStencilStateCreateInfo*)graphics_pipeline_create_info.pDepthStencilState;

//     auto* depth_stencil_state_info = (VkPipelineDepthStencilStateCreateInfo*)memory_resource.allocate(sizeof(VkPipelineDepthStencilStateCreateInfo));
//     *depth_stencil_state_info = defaults::depth_stencil_state_create_info_off;

//     graphics_pipeline_create_info.pDepthStencilState = depth_stencil_state_info;

//     return depth_stencil_state_info;
// }

// MAYBEUNUSED VkPipelineMultisampleStateCreateInfo* pipeline_info::get_multisample_state()
// {
//     if (graphics_pipeline_create_info.pMultisampleState != nullptr)
//         return (VkPipelineMultisampleStateCreateInfo*)graphics_pipeline_create_info.pMultisampleState;

//     auto* multisample_state_info = (VkPipelineMultisampleStateCreateInfo*)memory_resource.allocate(sizeof(VkPipelineMultisampleStateCreateInfo));
//     *multisample_state_info = defaults::multisample_state_create_info;

//     graphics_pipeline_create_info.pMultisampleState = multisample_state_info;

//     return multisample_state_info;
// }

// MAYBEUNUSED VkPipelineTessellationStateCreateInfo* pipeline_info::get_tessellation_state()
// {
//     if (graphics_pipeline_create_info.pTessellationState != nullptr)
//         return (VkPipelineTessellationStateCreateInfo*)graphics_pipeline_create_info.pTessellationState;

//     auto* tessellation_state_info = (VkPipelineTessellationStateCreateInfo*)memory_resource.allocate(sizeof(VkPipelineTessellationStateCreateInfo));
//     *tessellation_state_info = defaults::tessellation_state_create_info;

//     graphics_pipeline_create_info.pTessellationState = tessellation_state_info;

//     return tessellation_state_info;
// }