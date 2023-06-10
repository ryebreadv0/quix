#ifndef _QUIX_WINDOW_CPP
#define _QUIX_WINDOW_CPP

#include "quix_window.hpp"

namespace quix {

window::window(const char* title, int width, int height)
    : framebuffer_resized(false)
    , m_window(nullptr)
    , key_callback(nullptr)
    , cursor_callback(nullptr)
    , mouse_button_callback(nullptr)
{
    quix_assert(glfwInit() != GLFW_FALSE, "failed to initialize GLFW");

    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    m_window = glfwCreateWindow(width, height, title, nullptr, nullptr);

    quix_assert(m_window != nullptr, "failed to create GLFW window");

    glfwSetWindowUserPointer(m_window, this);

    glfwSetFramebufferSizeCallback(m_window, [](GLFWwindow* p_window, int  /*width*/, int /*height*/) {
        auto* instance = static_cast<window*>(glfwGetWindowUserPointer(p_window));
        instance->framebuffer_resized = true;
    });
}

window::~window()
{
    glfwDestroyWindow(m_window);
    glfwTerminate();
}

void window::enable_key_callback()
{
    glfwSetKeyCallback(m_window, [](GLFWwindow* p_window, int key, int scancode, int action, int mods) {
        auto* instance = static_cast<window*>(glfwGetWindowUserPointer(p_window));
        if (instance->key_callback) {
            instance->key_callback(p_window, key, scancode, action, mods);
        }
    });
}

void window::enable_cursor_callback()
{
    glfwSetCursorPosCallback(m_window, [](GLFWwindow* p_window, double xpos, double ypos) {
        auto* instance = static_cast<window*>(glfwGetWindowUserPointer(p_window));
        if (instance->cursor_callback) {
            instance->cursor_callback(p_window, xpos, ypos);
        }
    });
}

void window::enable_mouse_button_callback()
{
    glfwSetMouseButtonCallback(m_window, [](GLFWwindow* p_window, int button, int action, int mods) {
        auto* instance = static_cast<window*>(glfwGetWindowUserPointer(p_window));
        if (instance->mouse_button_callback) {
            instance->mouse_button_callback(p_window, button, action, mods);
        }
    });
}

} // namespace quix

#endif // _QUIX_WINDOW_CPP