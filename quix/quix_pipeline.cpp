#ifndef _QUIX_PIPELINE_CPP
#define _QUIX_PIPELINE_CPP

#include "quix_pipeline.hpp"

#include <utility>

#include "quix_device.hpp"
#include "quix_instance.hpp"
#include "quix_render_target.hpp"
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

    // pipeline_builder class
    
    pipeline_builder::pipeline_builder(std::shared_ptr<device> s_device, std::shared_ptr<render_target> s_render_target, pipeline_manager* pipeline_manager)
        : m_device(std::move(s_device))
        , m_render_target(std::move(s_render_target))
        , m_pipeline_manager(pipeline_manager)
    {
        pipeline_create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipeline_create_info.basePipelineHandle = nullptr;
        pipeline_create_info.basePipelineIndex = -1;

        init_pipeline_defaults();
    }

    NODISCARD std::shared_ptr<pipeline> pipeline_builder::create_graphics_pipeline()
    {
        create_pipeline_layout_info();

        return m_pipeline_manager->allocate_shared<pipeline>(m_device, m_render_target, &m_layout_info, &pipeline_create_info);

        // return pipeline { m_device, m_render_target, &m_layout_info, &pipeline_create_info };
    }

    VkPipelineShaderStageCreateInfo pipeline_builder::create_shader_stage(
        const char* file_path, const VkShaderStageFlagBits shader_stage)
    {
        EShLanguage EShStage{};
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
            quix_error("invalid shader stage");
        }

        shader shader_obj(file_path, EShStage);
        VkShaderModule shader_module = shader_obj.createShaderModule(m_device->get_logical_device());

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

    // pipeline_builder end

    // pipeline class

    pipeline::pipeline(std::shared_ptr<device> device,
        std::shared_ptr<render_target> render_target,
        const VkPipelineLayoutCreateInfo* pipeline_layout_info,
        VkGraphicsPipelineCreateInfo* pipeline_create_info)
        : m_device(std::move(device))
        , m_render_target(std::move(render_target))
    {
        create_pipeline_layout(pipeline_layout_info);
        create_pipeline(pipeline_create_info);
        for (int i = 0; i < pipeline_create_info->stageCount; i++) {
            vkDestroyShaderModule(m_device->get_logical_device(), pipeline_create_info->pStages[i].module, nullptr);
        }
    }

    pipeline::~pipeline()
    {
        vkDestroyPipelineLayout(m_device->get_logical_device(), m_pipeline_layout, nullptr);
        vkDestroyPipeline(m_device->get_logical_device(), m_pipeline, nullptr);
    }

    void pipeline::create_pipeline_layout(const VkPipelineLayoutCreateInfo* pipeline_layout_info)
    {
        if (pipeline_layout_info == nullptr) {
            pipeline_layout_info = &defaults::layout_create_info;
        }

        VK_CHECK(vkCreatePipelineLayout(m_device->get_logical_device(), pipeline_layout_info, nullptr, &m_pipeline_layout), "failed to create pipeline layout");
    }

    void pipeline::create_pipeline(VkGraphicsPipelineCreateInfo* pipeline_create_info)
    {
        pipeline_create_info->layout = m_pipeline_layout;
        pipeline_create_info->renderPass = m_render_target->get_render_pass();

        VK_CHECK(vkCreateGraphicsPipelines(m_device->get_logical_device(), VK_NULL_HANDLE, 1, pipeline_create_info, nullptr, &m_pipeline), "failed to create graphics pipeline");
    }

    // pipeline class end

    // pipeline_manager class

    pipeline_manager::pipeline_manager(std::shared_ptr<device> device)
        : m_device(std::move(device))
    {
    }

    std::shared_ptr<pipeline_builder> pipeline_manager::create_pipeline_builder(std::shared_ptr<render_target> render_target)
    {
        return allocate_shared<pipeline_builder>(m_device, std::move(render_target), this);
        // return pipeline_builder { m_device, render_target };
    }

    // pipeline_manager class end

} // namespace graphics

} // namespace quix

#endif // _QUIX_PIPELINE_CPP
