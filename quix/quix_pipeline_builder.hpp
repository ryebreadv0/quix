#ifndef _QUIX_PIPELINE_BUILDER_HPP
#define _QUIX_PIPELINE_BUILDER_HPP

#include <vulkan/vulkan_core.h>
namespace quix {

class device;
class render_target;

namespace graphics {
    class pipeline_manager;

    class pipeline {
        friend class pipeline_builder;

    public:
        pipeline(weakref<device> p_device,
            weakref<render_target> p_render_target,
            const VkPipelineLayoutCreateInfo* pipeline_layout_info,
            VkGraphicsPipelineCreateInfo* pipeline_create_info);
        ~pipeline();

        pipeline(const pipeline&) = delete;
        pipeline& operator=(const pipeline&) = delete;
        pipeline(pipeline&&) = delete;
        pipeline& operator=(pipeline&&) = delete;

        NODISCARD inline VkPipelineLayout get_layout() const noexcept { return m_pipeline_layout; }
        NODISCARD inline VkPipeline get_pipeline() const noexcept { return m_pipeline; }

    private:
        weakref<device> m_device;
        weakref<render_target> m_render_target;

        VkPipelineLayout m_pipeline_layout = VK_NULL_HANDLE;
        VkPipeline m_pipeline = VK_NULL_HANDLE;

        void create_pipeline_layout(const VkPipelineLayoutCreateInfo* pipeline_layout_info);
        void create_pipeline(VkGraphicsPipelineCreateInfo* pipeline_create_info);
    };

    class pipeline_builder {
        friend class pipeline_manager;

    public:
        pipeline_builder(weakref<device> p_device, weakref<render_target> p_render_target, weakref<pipeline_manager> p_pipeline_manager);

        NODISCARD VkPipelineShaderStageCreateInfo create_shader_stage(
            const char* file_path, const VkShaderStageFlagBits shader_stage);

        NODISCARD std::shared_ptr<pipeline> create_graphics_pipeline();

    private:
        struct pipeline_info {
            VkPipelineVertexInputStateCreateInfo vertex_input_state;
            VkPipelineInputAssemblyStateCreateInfo input_assembly_state;
            VkPipelineTessellationStateCreateInfo tessellation_state;
            VkPipelineViewportStateCreateInfo viewport_state;
            VkPipelineRasterizationStateCreateInfo rasterization_state;
            VkPipelineMultisampleStateCreateInfo multisample_state;
            VkPipelineDepthStencilStateCreateInfo depth_stencil_state;
            VkPipelineColorBlendAttachmentState color_blend_attachment_state;
            VkPipelineColorBlendStateCreateInfo color_blend_state;
            VkPipelineDynamicStateCreateInfo dynamic_state;

            VkPushConstantRange push_constant_range {};
            std::array<VkDescriptorSetLayout, 4> descriptor_set_layouts {};
            uint32_t descriptor_set_layout_count {};
        };

        weakref<device> m_device;
        weakref<render_target> m_render_target;
        weakref<pipeline_manager> m_pipeline_manager;

        pipeline_info info;
        VkPipelineLayoutCreateInfo m_layout_info {};
        VkGraphicsPipelineCreateInfo pipeline_create_info {};

        inline void init_pipeline_defaults()
        {
            create_vertex_state(nullptr, 0, nullptr, 0);
            create_input_assembly(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP);
            create_viewport_state(nullptr, 1, nullptr, 1);
            create_rasterization_state(VK_POLYGON_MODE_FILL, VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_CLOCKWISE);
            create_multisample_state(VK_SAMPLE_COUNT_1_BIT);
            create_depth_stencil_state(VK_FALSE, VK_FALSE, VK_COMPARE_OP_LESS_OR_EQUAL);
            create_color_blend_attachment_state(VK_FALSE, VK_BLEND_FACTOR_SRC_ALPHA, VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA, VK_BLEND_OP_ADD);
            create_color_blend_state(VK_FALSE, VK_LOGIC_OP_COPY, &info.color_blend_attachment_state, 1);
            create_dynamic_state(dynamic_states_default.data(), dynamic_states_default.size());
        }

        inline void create_pipeline_layout_info()
        {
            m_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
            m_layout_info.pNext = nullptr;
            m_layout_info.flags = 0;
            m_layout_info.setLayoutCount = info.descriptor_set_layout_count;
            m_layout_info.pSetLayouts = info.descriptor_set_layouts.data();
            // count is set by add_push_constant
            m_layout_info.pPushConstantRanges = &info.push_constant_range;
        }

    public:
        inline pipeline_builder& add_shader_stages(
            VkPipelineShaderStageCreateInfo* stages, const uint32_t stage_count)
        {
            pipeline_create_info.stageCount = stage_count;
            pipeline_create_info.pStages = stages;

            return *this;
        }

        template <std::size_t stage_count>
        inline pipeline_builder& add_shader_stages(
            std::array<VkPipelineShaderStageCreateInfo, stage_count>& stages)
        {
            pipeline_create_info.stageCount = stage_count;
            pipeline_create_info.pStages = stages.data();

            return *this;
        }

        inline pipeline_builder& create_vertex_state(
            const VkVertexInputBindingDescription* bindings, const uint32_t binding_count,
            const VkVertexInputAttributeDescription* attributes, const uint32_t arribute_count)
        {
            info.vertex_input_state = VkPipelineVertexInputStateCreateInfo {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .vertexBindingDescriptionCount = binding_count,
                .pVertexBindingDescriptions = bindings,
                .vertexAttributeDescriptionCount = arribute_count,
                .pVertexAttributeDescriptions = attributes
            };

            pipeline_create_info.pVertexInputState = &info.vertex_input_state;

            return *this;
        }

        inline pipeline_builder& create_input_assembly(VkPrimitiveTopology topology)
        {
            info.input_assembly_state = VkPipelineInputAssemblyStateCreateInfo {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .topology = topology,
                .primitiveRestartEnable = VK_FALSE
            };

            pipeline_create_info.pInputAssemblyState = &info.input_assembly_state;

            return *this;
        }

        inline pipeline_builder& create_dynamic_state(
            const VkDynamicState* states, const uint32_t state_count)
        {
            info.dynamic_state = VkPipelineDynamicStateCreateInfo {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .dynamicStateCount = state_count,
                .pDynamicStates = states
            };

            pipeline_create_info.pDynamicState = &info.dynamic_state;

            return *this;
        }

        inline pipeline_builder& create_viewport_state(
            const VkViewport* viewports, const uint32_t viewport_count,
            const VkRect2D* scissors, const uint32_t scissor_count)
        {
            info.viewport_state = VkPipelineViewportStateCreateInfo {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .viewportCount = viewport_count,
                .pViewports = viewports,
                .scissorCount = scissor_count,
                .pScissors = scissors
            };

            pipeline_create_info.pViewportState = &info.viewport_state;

            return *this;
        }

        inline pipeline_builder& create_rasterization_state(
            const VkPolygonMode polygon_mode, const VkCullModeFlags cull_mode, const VkFrontFace front_face)
        {
            info.rasterization_state = VkPipelineRasterizationStateCreateInfo {
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

            pipeline_create_info.pRasterizationState = &info.rasterization_state;

            return *this;
        }

        inline pipeline_builder& create_multisample_state(
            const VkSampleCountFlagBits sample_count)
        {
            info.multisample_state = VkPipelineMultisampleStateCreateInfo {
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

            pipeline_create_info.pMultisampleState = &info.multisample_state;

            return *this;
        }

        inline pipeline_builder& create_depth_stencil_state(
            const VkBool32 depth_test, const VkBool32 depth_write, const VkCompareOp depth_compare_op)
        {
            info.depth_stencil_state = VkPipelineDepthStencilStateCreateInfo {
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

            pipeline_create_info.pDepthStencilState = &info.depth_stencil_state;

            return *this;
        }

        inline pipeline_builder& create_color_blend_attachment_state(
            const VkBool32 blend_enable, const VkBlendFactor src_blend_factor,
            const VkBlendFactor dst_blend_factor, const VkBlendOp color_blend_op,
            const VkColorComponentFlags color_write_mask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT)
        {
            info.color_blend_attachment_state = VkPipelineColorBlendAttachmentState {
                .blendEnable = blend_enable,
                .srcColorBlendFactor = src_blend_factor,
                .dstColorBlendFactor = dst_blend_factor,
                .colorBlendOp = color_blend_op,
                .srcAlphaBlendFactor = src_blend_factor,
                .dstAlphaBlendFactor = dst_blend_factor,
                .alphaBlendOp = color_blend_op,
                .colorWriteMask = color_write_mask
            };

            pipeline_create_info.pColorBlendState = &info.color_blend_state;

            return *this;
        }

        inline pipeline_builder& create_color_blend_attachment_state(
            const VkBool32 blend_enable, const VkBlendFactor src_color_blend_factor,
            const VkBlendFactor dst_color_blend_factor, const VkBlendOp color_blend_op,
            const VkBlendFactor src_alpha_blend_factor, const VkBlendFactor dst_alpha_blend_factor,
            const VkBlendOp alpha_blend_op, const VkColorComponentFlags color_write_mask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT)
        {
            info.color_blend_attachment_state = VkPipelineColorBlendAttachmentState {
                .blendEnable = blend_enable,
                .srcColorBlendFactor = src_color_blend_factor,
                .dstColorBlendFactor = dst_color_blend_factor,
                .colorBlendOp = color_blend_op,
                .srcAlphaBlendFactor = src_alpha_blend_factor,
                .dstAlphaBlendFactor = dst_alpha_blend_factor,
                .alphaBlendOp = alpha_blend_op,
                .colorWriteMask = color_write_mask
            };

            return *this;
        }

        static constexpr float blend_constants_default[] = { 0.0f, 0.0f, 0.0f, 0.0f };

        inline pipeline_builder& create_color_blend_state(
            const VkBool32 logic_op_enable, const VkLogicOp logic_op,
            const VkPipelineColorBlendAttachmentState* attachments, const uint32_t attachment_count,
            const float blend_constants[4])
        {
            info.color_blend_state = VkPipelineColorBlendStateCreateInfo {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .logicOpEnable = logic_op_enable,
                .logicOp = logic_op,
                .attachmentCount = attachment_count,
                .pAttachments = attachments,
                .blendConstants = { blend_constants[0], blend_constants[1], blend_constants[2], blend_constants[3] }
            };

            pipeline_create_info.pColorBlendState = &info.color_blend_state;

            return *this;
        }

        inline pipeline_builder& create_color_blend_state(
            const VkBool32 logic_op_enable, const VkLogicOp logic_op,
            const VkPipelineColorBlendAttachmentState* attachments, const uint32_t attachment_count)
        {
            info.color_blend_state = VkPipelineColorBlendStateCreateInfo {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .logicOpEnable = logic_op_enable,
                .logicOp = logic_op,
                .attachmentCount = attachment_count,
                .pAttachments = attachments,
                .blendConstants = { blend_constants_default[0], blend_constants_default[1], blend_constants_default[2], blend_constants_default[3] }
            };

            pipeline_create_info.pColorBlendState = &info.color_blend_state;

            return *this;
        }

        inline pipeline_builder& create_tesselation_state(
            const uint32_t control_points)
        {
            info.tessellation_state = VkPipelineTessellationStateCreateInfo {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .patchControlPoints = control_points
            };

            pipeline_create_info.pTessellationState = &info.tessellation_state;

            return *this;
        }

        inline pipeline_builder& add_push_constant(VkShaderStageFlags shader_flags, uint32_t size) noexcept
        {
            info.push_constant_range = VkPushConstantRange {
                .stageFlags = shader_flags,
                .offset = 0,
                .size = size
            };

            m_layout_info.pushConstantRangeCount = 1;
            return *this;
        }

        inline pipeline_builder& add_descriptor_set_layout(VkDescriptorSetLayout layout) noexcept
        {
            if (info.descriptor_set_layout_count >= 3) {
                spdlog::error("descriptor set layout count exceeded, max is 4");
                return *this;
            }

            info.descriptor_set_layouts[info.descriptor_set_layout_count] = layout;
            ++info.descriptor_set_layout_count;

            return *this;
        }

        inline pipeline_builder& add_descriptor_set_layout(std::initializer_list<VkDescriptorSetLayout> layouts) noexcept
        {

            for (auto& layout : layouts) {
                if (info.descriptor_set_layout_count >= 3) {
                    spdlog::error("descriptor set layout count exceeded, max is 4");
                    return *this;
                }

                info.descriptor_set_layouts[info.descriptor_set_layout_count] = layout;
                ++info.descriptor_set_layout_count;
            }

            return *this;
        }

        template <typename... Args>
        NODISCARD static inline constexpr auto create_shader_array(Args&&... args)
        {
            return std::array<VkPipelineShaderStageCreateInfo, sizeof...(Args)> { std::forward<Args>(args)... };
        }

        static constexpr std::array<VkDynamicState, 2> dynamic_states_default = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };

    }; // class pipeline_builder

} // namespace graphics

} // namespace quix

#endif // QUIX_VULKAN_PIPELINE_BUILDER_HPP