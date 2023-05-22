#include "quix_device.hpp"

static constexpr int WIDTH = 800;
static constexpr int HEIGHT = 600;

int main()
{


    quix::device device("quix_example", VK_MAKE_VERSION(0, 0, 1));
    
    auto window = device.window();



    while (glfwWindowShouldClose(window) == GLFW_FALSE)
    {
        glfwPollEvents();
        glfwSwapBuffers(window);
    }


    
    glfwTerminate();
    
    return 0;
}