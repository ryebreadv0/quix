#ifndef _QUIX_PIPELINE_HPP
#define _QUIX_PIPELINE_HPP

#include "quix_instance.hpp"

namespace quix {

namespace graphics {

struct renderpass_info {
    std::pmr::monotonic_buffer_resource memory_resource{};

    std::vector<VkAttachmentDescription, std::pmr::polymorphic_allocator<VkAttachmentDescription>> attachments{&memory_resource};
    std::vector<VkAttachmentReference, std::pmr::polymorphic_allocator<VkAttachmentReference>> attachments_references{&memory_resource};
    std::vector<VkSubpassDescription, std::pmr::polymorphic_allocator<VkSubpassDescription>> subpasses{&memory_resource};
    std::vector<VkSubpassDependency, std::pmr::polymorphic_allocator<VkSubpassDependency>> subpass_dependencies{&memory_resource};
};

// maybe instead of this I just use an allocator and bind directly to the graphics pipeline struct
// VkPipelineVertexInputStateCreateInfo vertex_input_state_create_info;
// VkPipelineInputAssemblyStateCreateInfo input_assembly_state_create_info;

// VkPipelineViewportStateCreateInfo viewport_state_create_info;
// VkPipelineDynamicStateCreateInfo dynamic_state_create_info;

// VkPipelineRasterizationStateCreateInfo rasterization_state_create_info;

// VkPipelineColorBlendAttachmentState color_blend_attachment_state;
// VkPipelineColorBlendStateCreateInfo color_blend_state_create_info;

// VkPipelineDepthStencilStateCreateInfo depth_stencil_state_create_info;
// VkPipelineMultisampleStateCreateInfo multisample_state_create_info;

struct pipeline_info {
    
    std::pmr::monotonic_buffer_resource memory_resource;

    VkGraphicsPipelineCreateInfo graphics_pipeline_create_info{};

    void init_defaults();

    void set_shader_stage_create_info(uint32_t stage_count);
    MAYBEUNUSED void create_shader_stage_create_info(VkDevice device, uint32_t index, const char* file_path, VkShaderStageFlagBits shader_stage);

    MAYBEUNUSED VkPipelineVertexInputStateCreateInfo* get_vertex_input_state();
    MAYBEUNUSED VkPipelineInputAssemblyStateCreateInfo* get_input_assembly_state();

    MAYBEUNUSED VkPipelineViewportStateCreateInfo* get_viewport_state();
    MAYBEUNUSED VkPipelineDynamicStateCreateInfo* get_dynamic_state();

    MAYBEUNUSED VkPipelineRasterizationStateCreateInfo* get_rasterization_state();

    MAYBEUNUSED VkPipelineColorBlendAttachmentState* get_color_blend_attachment_state();
    MAYBEUNUSED VkPipelineColorBlendStateCreateInfo* get_color_blend_state();

    MAYBEUNUSED VkPipelineDepthStencilStateCreateInfo* get_depth_stencil_state();
    MAYBEUNUSED VkPipelineMultisampleStateCreateInfo* get_multisample_state();

    MAYBEUNUSED VkPipelineTessellationStateCreateInfo* get_tessellation_state();

};

struct pipeline {
    VkPipeline pipeline;
    VkPipelineLayout pipeline_layout;
    VkRenderPass render_pass;

    

    void create_pipeline_layout(VkDevice device, const VkPipelineLayoutCreateInfo* pipeline_create_info);
    void create_renderpass(VkDevice device, const renderpass_info* renderpass_info);
    void create_pipeline(VkDevice device, pipeline_info* pipeline_info);
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