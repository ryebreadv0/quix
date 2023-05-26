#ifndef _QUIX_PIPELINE_HPP
#define _QUIX_PIPELINE_HPP

#include "quix_instance.hpp"

#include "quix_pipeline_builder.hpp"

namespace quix {

namespace graphics {

    struct renderpass_info {
    private:
        std::pmr::monotonic_buffer_resource memory_resource {};

    public:
        std::vector<VkAttachmentDescription, std::pmr::polymorphic_allocator<VkAttachmentDescription>> attachments { &memory_resource };
        std::vector<VkAttachmentReference, std::pmr::polymorphic_allocator<VkAttachmentReference>> attachments_references { &memory_resource };
        std::vector<VkSubpassDescription, std::pmr::polymorphic_allocator<VkSubpassDescription>> subpasses { &memory_resource };
        std::vector<VkSubpassDependency, std::pmr::polymorphic_allocator<VkSubpassDependency>> subpass_dependencies { &memory_resource };
    };

    // struct pipeline_info {
    //     VkPipelineShaderStageCreateInfo* shader_stages = nullptr;
    //     uint32_t shader_stage_count = 0;
    //     VkPipelineVertexInputStateCreateInfo vertex_input_state = pipeline_builder::create_vertex_state(nullptr, 0, nullptr, 0);
    //     VkPipelineInputAssemblyStateCreateInfo input_assembly_state = pipeline_builder::create_input_assembly(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP);
    //     VkPipelineTessellationStateCreateInfo tessellation_state = {};
    //     VkPipelineViewportStateCreateInfo viewport_state = pipeline_builder::create_viewport_state(nullptr, 0, nullptr, 0);
    //     VkPipelineRasterizationStateCreateInfo rasterization_state = pipeline_builder::create_rasterization_state(VK_POLYGON_MODE_FILL, VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_CLOCKWISE);
    //     VkPipelineMultisampleStateCreateInfo multisample_state = pipeline_builder::create_multisample_state(VK_SAMPLE_COUNT_1_BIT);
    //     VkPipelineDepthStencilStateCreateInfo depth_stencil_state = pipeline_builder::create_depth_stencil_state(VK_FALSE, VK_FALSE, VK_COMPARE_OP_LESS_OR_EQUAL);
    //     VkPipelineColorBlendAttachmentState color_blend_attachment_state = pipeline_builder::create_color_blend_attachment_state(VK_FALSE, VK_BLEND_FACTOR_SRC_ALPHA, VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA, VK_BLEND_OP_ADD);
    //     VkPipelineColorBlendStateCreateInfo color_blend_state = pipeline_builder::create_color_blend_state(VK_FALSE, VK_LOGIC_OP_COPY, &color_blend_attachment_state, 1);
    //     VkPipelineDynamicStateCreateInfo dynamic_state = pipeline_builder::create_dynamic_state(dynamic_states_default.data(), dynamic_states_default.size());
    // };

    struct pipeline {
        VkPipeline pipeline;
        VkPipelineLayout pipeline_layout;
        VkRenderPass render_pass;

        void create_pipeline_layout(VkDevice device, const VkPipelineLayoutCreateInfo* pipeline_create_info);
        void create_renderpass(VkDevice device, const renderpass_info* renderpass_info);
        void create_pipeline(VkDevice device, const VkGraphicsPipelineCreateInfo* pipeline_create_info);
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