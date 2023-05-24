#include "quix_instance.hpp"
#include "quix_shader.hpp"

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

    quix::shader shader("examples/simpleshader.frag", EShLangFragment);
    
    auto* window = instance.window();

    while (glfwWindowShouldClose(window) == GLFW_FALSE)
    {
        glfwPollEvents();
    }


        
    return 0;
}