#include "quix_instance.hpp"
#include "quix_pipeline.hpp"
#include "quix_shader.hpp"

static constexpr int WIDTH = 800;
static constexpr int HEIGHT = 600;

int main()
{
    quix::instance instance("quix_example",
        VK_MAKE_VERSION(0, 0, 1),
        WIDTH, HEIGHT);

    instance.create_device({ VK_KHR_SWAPCHAIN_EXTENSION_NAME },
        { .tessellationShader = VK_TRUE });

    instance.create_swapchain(2, VK_PRESENT_MODE_FIFO_KHR);

    auto device = instance.get_logical_device();

    quix::graphics::pipeline_builder pipeline_builder(device);
    
    auto shader_stages = quix::graphics::pipeline_builder::create_shader_array(
        pipeline_builder.create_shader_stage("examples/simpleshader.frag", VK_SHADER_STAGE_FRAGMENT_BIT)
    );

    pipeline_builder.create_shader_stages(shader_stages.data(), shader_stages.size());


    // quix::graphics::pipeline_info pipeline_info {};

    // auto shader_stages = quix::graphics::create_shader_stages(
    //     quix::graphics::create_shader_stage(device, "examples/simpleshader.frag", VK_SHADER_STAGE_FRAGMENT_BIT)
    // );

    // VkPushConstantRange push_constant_range {
    //     .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
    //     .offset = 0,
    //     .size = sizeof(float) * 4
    // };

    // auto pipeline_layout = quix::graphics::create_pipeline_layout_info(
    //     nullptr, 0, &push_constant_range, 1);

    auto* window = instance.window();

    while (glfwWindowShouldClose(window) == GLFW_FALSE) {
        glfwPollEvents();
    }

    return 0;
}