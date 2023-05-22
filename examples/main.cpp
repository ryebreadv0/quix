#include "quix_device.hpp"

static constexpr int WIDTH = 800;
static constexpr int HEIGHT = 600;

int main()
{
    quix::device device("quix_example", 
        VK_MAKE_VERSION(0, 0, 1), 
        WIDTH, HEIGHT,
        {VK_KHR_SWAPCHAIN_EXTENSION_NAME}, {.tessellationShader = VK_TRUE}
    );

    device.create_swapchain(2, VK_PRESENT_MODE_FIFO_KHR);
    
    auto* window = device.window();

    while (glfwWindowShouldClose(window) == GLFW_FALSE)
    {
        glfwPollEvents();
    }


        
    return 0;
}