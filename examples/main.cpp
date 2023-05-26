#include "quix_instance.hpp"
#include "quix_shader.hpp"
#include "quix_pipeline.hpp"



static constexpr int WIDTH = 800;
static constexpr int HEIGHT = 600;



int main()
{
    quix::instance instance("quix_example", 
        VK_MAKE_VERSION(0, 0, 1), 
        WIDTH, HEIGHT
    );

    instance.create_device( {VK_KHR_SWAPCHAIN_EXTENSION_NAME},
        {.tessellationShader = VK_TRUE}
    );

    instance.create_swapchain(2, VK_PRESENT_MODE_FIFO_KHR);

    quix::graphics::pipeline_info pipeline_info{};
    // pipeline_info.init_defaults();

    // pipeline_info.allocate_shader_stages(1);
    // pipeline_info.setup_shader_stage(instance.get_logical_device(), 0, "examples/simpleshader.frag", VK_SHADER_STAGE_FRAGMENT_BIT);


    

    
    auto* window = instance.window();

    while (glfwWindowShouldClose(window) == GLFW_FALSE)
    {
        glfwPollEvents();
    }


        
    return 0;
}