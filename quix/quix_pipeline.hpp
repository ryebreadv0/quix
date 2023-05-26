#ifndef _QUIX_PIPELINE_HPP
#define _QUIX_PIPELINE_HPP

#include "quix_instance.hpp"

namespace quix {

namespace graphics {

    VkPipelineShaderStageCreateInfo create_shader_stage(
        const VkDevice device, const char* file_path, const VkShaderStageFlagBits shader_stage);

    template <typename... Args>
    inline constexpr auto create_shader_stages(Args&&... args)
    {
        return std::array<VkPipelineShaderStageCreateInfo, sizeof...(Args)> { std::forward<Args>(args)... };
    }

    inline constexpr VkPipelineVertexInputStateCreateInfo create_vertex_state(
        const VkVertexInputBindingDescription* bindings, const uint32_t binding_count,
        const VkVertexInputAttributeDescription* attributes, const uint32_t arribute_count)
    {
        return VkPipelineVertexInputStateCreateInfo {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .vertexBindingDescriptionCount = binding_count,
            .pVertexBindingDescriptions = bindings,
            .vertexAttributeDescriptionCount = arribute_count,
            .pVertexAttributeDescriptions = attributes
        };
    }

    inline consteval VkPipelineInputAssemblyStateCreateInfo create_input_assembly(VkPrimitiveTopology topology)
    {
        return VkPipelineInputAssemblyStateCreateInfo {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .topology = topology,
            .primitiveRestartEnable = VK_FALSE
        };
    }

    inline constexpr VkPipelineDynamicStateCreateInfo create_dynamic_state(
        const VkDynamicState* states, const uint32_t state_count)
    {
        return VkPipelineDynamicStateCreateInfo {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .dynamicStateCount = state_count,
            .pDynamicStates = states
        };
    }

    inline constexpr VkPipelineViewportStateCreateInfo create_viewport_state(
        const VkViewport* viewports, const uint32_t viewport_count,
        const VkRect2D* scissors, const uint32_t scissor_count)
    {
        return VkPipelineViewportStateCreateInfo {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .viewportCount = viewport_count,
            .pViewports = viewports,
            .scissorCount = scissor_count,
            .pScissors = scissors
        };
    }

    inline consteval VkPipelineRasterizationStateCreateInfo create_rasterization_state(
        const VkPolygonMode polygon_mode, const VkCullModeFlags cull_mode, const VkFrontFace front_face)
    {
        return VkPipelineRasterizationStateCreateInfo {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .depthClampEnable = VK_FALSE,
            .rasterizerDiscardEnable = VK_FALSE,
            .polygonMode = polygon_mode,
            .cullMode = cull_mode,
            .frontFace = front_face,
            .depthBiasEnable = VK_FALSE,
            .depthBiasConstantFactor = 0.0f,
            .depthBiasClamp = 0.0f,
            .depthBiasSlopeFactor = 0.0f,
            .lineWidth = 1.0f
        };
    }

    inline consteval VkPipelineMultisampleStateCreateInfo create_multisample_state(
        const VkSampleCountFlagBits sample_count)
    {
        return VkPipelineMultisampleStateCreateInfo {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .rasterizationSamples = sample_count,
            .sampleShadingEnable = VK_FALSE,
            .minSampleShading = 1.0f,
            .pSampleMask = nullptr,
            .alphaToCoverageEnable = VK_FALSE,
            .alphaToOneEnable = VK_FALSE
        };
    }

    inline consteval VkPipelineDepthStencilStateCreateInfo create_depth_stencil_state(
        const VkBool32 depth_test, const VkBool32 depth_write, const VkCompareOp depth_compare_op)
    {
        return VkPipelineDepthStencilStateCreateInfo {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .depthTestEnable = depth_test,
            .depthWriteEnable = depth_write,
            .depthCompareOp = depth_compare_op,
            .depthBoundsTestEnable = VK_FALSE,
            .stencilTestEnable = VK_FALSE,
            .front = VkStencilOpState {},
            .back = VkStencilOpState {},
            .minDepthBounds = 0.0f,
            .maxDepthBounds = 1.0f
        };
    }

    inline consteval VkPipelineColorBlendAttachmentState create_color_blend_attachment_state(
        const VkBool32 blend_enable, const VkBlendFactor src_blend_factor,
        const VkBlendFactor dst_blend_factor, const VkBlendOp color_blend_op,
        const VkColorComponentFlags color_write_mask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT)
    {
        return VkPipelineColorBlendAttachmentState {
            .blendEnable = blend_enable,
            .srcColorBlendFactor = src_blend_factor,
            .dstColorBlendFactor = dst_blend_factor,
            .colorBlendOp = color_blend_op,
            .srcAlphaBlendFactor = src_blend_factor,
            .dstAlphaBlendFactor = dst_blend_factor,
            .alphaBlendOp = color_blend_op,
            .colorWriteMask = color_write_mask
        };
    }

    inline consteval VkPipelineColorBlendAttachmentState create_color_blend_attachment_state(
        const VkBool32 blend_enable, const VkBlendFactor src_color_blend_factor,
        const VkBlendFactor dst_color_blend_factor, const VkBlendOp color_blend_op,
        const VkBlendFactor src_alpha_blend_factor, const VkBlendFactor dst_alpha_blend_factor,
        const VkBlendOp alpha_blend_op, const VkColorComponentFlags color_write_mask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT)
    {
        return VkPipelineColorBlendAttachmentState {
            .blendEnable = blend_enable,
            .srcColorBlendFactor = src_color_blend_factor,
            .dstColorBlendFactor = dst_color_blend_factor,
            .colorBlendOp = color_blend_op,
            .srcAlphaBlendFactor = src_alpha_blend_factor,
            .dstAlphaBlendFactor = dst_alpha_blend_factor,
            .alphaBlendOp = alpha_blend_op,
            .colorWriteMask = color_write_mask
        };
    }

    static constexpr float blend_constants_default[] = { 0.0f, 0.0f, 0.0f, 0.0f };

    inline constexpr VkPipelineColorBlendStateCreateInfo create_color_blend_state(
        const VkBool32 logic_op_enable, const VkLogicOp logic_op,
        const VkPipelineColorBlendAttachmentState* attachments, const uint32_t attachment_count,
        const float blend_constants[4])
    {
        return VkPipelineColorBlendStateCreateInfo {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .logicOpEnable = logic_op_enable,
            .logicOp = logic_op,
            .attachmentCount = attachment_count,
            .pAttachments = attachments,
            .blendConstants = { blend_constants[0], blend_constants[1], blend_constants[2], blend_constants[3] }
        };
    }

    inline constexpr VkPipelineColorBlendStateCreateInfo create_color_blend_state(
        const VkBool32 logic_op_enable, const VkLogicOp logic_op,
        const VkPipelineColorBlendAttachmentState* attachments, const uint32_t attachment_count)
    {
        return VkPipelineColorBlendStateCreateInfo {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .logicOpEnable = logic_op_enable,
            .logicOp = logic_op,
            .attachmentCount = attachment_count,
            .pAttachments = attachments,
            .blendConstants = { blend_constants_default[0], blend_constants_default[1], blend_constants_default[2], blend_constants_default[3] }
        };
    }

    inline consteval VkPipelineTessellationStateCreateInfo create_tesselation_state(
        const uint32_t control_points)
    {
        return VkPipelineTessellationStateCreateInfo {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .patchControlPoints = control_points
        };
    }

    inline constexpr VkPipelineLayoutCreateInfo create_pipeline_layout_info(
        const VkDescriptorSetLayout* set_layout, const uint32_t set_layout_count,
        const VkPushConstantRange* push_constant_range, const uint32_t push_constant_count)
    {
        return {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .setLayoutCount = set_layout_count,
            .pSetLayouts = set_layout,
            .pushConstantRangeCount = push_constant_count,
            .pPushConstantRanges = push_constant_range
        };
    }

    template <typename... Args>
    inline consteval auto create_dynamic_states(Args&&... args)
    {
        return std::array<VkDynamicState, sizeof...(Args)> { std::forward<Args>(args)... };
    }

    static constexpr auto dynamic_states_default = create_dynamic_states(VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR);

    struct renderpass_info {
    private:
        std::pmr::monotonic_buffer_resource memory_resource {};

    public:
        std::vector<VkAttachmentDescription, std::pmr::polymorphic_allocator<VkAttachmentDescription>> attachments { &memory_resource };
        std::vector<VkAttachmentReference, std::pmr::polymorphic_allocator<VkAttachmentReference>> attachments_references { &memory_resource };
        std::vector<VkSubpassDescription, std::pmr::polymorphic_allocator<VkSubpassDescription>> subpasses { &memory_resource };
        std::vector<VkSubpassDependency, std::pmr::polymorphic_allocator<VkSubpassDependency>> subpass_dependencies { &memory_resource };
    };

    struct pipeline_info {
        VkPipelineShaderStageCreateInfo* shader_stages = nullptr;
        uint32_t shader_stage_count = 0;
        VkPipelineVertexInputStateCreateInfo vertex_input_state = create_vertex_state(nullptr, 0, nullptr, 0);
        VkPipelineInputAssemblyStateCreateInfo input_assembly_state = create_input_assembly(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP);
        VkPipelineTessellationStateCreateInfo tessellation_state = {};
        VkPipelineViewportStateCreateInfo viewport_state = create_viewport_state(nullptr, 0, nullptr, 0);
        VkPipelineRasterizationStateCreateInfo rasterization_state = create_rasterization_state(VK_POLYGON_MODE_FILL, VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_CLOCKWISE);
        VkPipelineMultisampleStateCreateInfo multisample_state = create_multisample_state(VK_SAMPLE_COUNT_1_BIT);
        VkPipelineDepthStencilStateCreateInfo depth_stencil_state = create_depth_stencil_state(VK_FALSE, VK_FALSE, VK_COMPARE_OP_LESS_OR_EQUAL);
        VkPipelineColorBlendAttachmentState color_blend_attachment_state = create_color_blend_attachment_state(VK_FALSE, VK_BLEND_FACTOR_SRC_ALPHA, VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA, VK_BLEND_OP_ADD);
        VkPipelineColorBlendStateCreateInfo color_blend_state = create_color_blend_state(VK_FALSE, VK_LOGIC_OP_COPY, &color_blend_attachment_state, 1);
        VkPipelineDynamicStateCreateInfo dynamic_state = create_dynamic_state(dynamic_states_default.data(), dynamic_states_default.size());
    };

    struct pipeline {
        VkPipeline pipeline;
        VkPipelineLayout pipeline_layout;
        VkRenderPass render_pass;

        void create_pipeline_layout(VkDevice device, const VkPipelineLayoutCreateInfo* pipeline_create_info);
        void create_renderpass(VkDevice device, const renderpass_info* renderpass_info);
        void create_pipeline(VkDevice device, pipeline_info* pipeline_info, VkPipeline pipeline_handle = VK_NULL_HANDLE, int32_t pipeline_index = -1);
    };

    class pipeline_manager {
    public:
        pipeline_manager();
        ~pipeline_manager();

        pipeline_manager(const pipeline_manager&) = delete;
        pipeline_manager& operator=(const pipeline_manager&) = delete;
        pipeline_manager(pipeline_manager&&) = delete;
        pipeline_manager& operator=(pipeline_manager&&) = delete;

        

    private:
        std::shared_ptr<device> s_device;
        std::shared_ptr<swapchain> s_swapchain;
    };

} // namespace graphics

} // namespace quix

#endif // _QUIX_PIPELINE_HPP