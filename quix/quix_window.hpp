#ifndef _QUIX_WINDOW_HPP
#define _QUIX_WINDOW_HPP

namespace quix {

class device;

class window {
    friend class device;

public:
    window(const char* title, int width, int height);
    ~window();

    window(const window&) = delete;
    window& operator=(const window&) = delete;
    window(window&&) = delete;
    window& operator=(window&&) = delete;

    NODISCARD inline bool should_close() const { return glfwWindowShouldClose(m_window); }
    inline void poll_events() const { glfwPollEvents(); }
    inline void wait_events() const { glfwWaitEvents(); }

    NODISCARD inline GLFWwindow* get_window() const noexcept { return m_window; }
    
    NODISCARD inline bool get_framebuffer_resized() noexcept
    {
        if (framebuffer_resized == true) {
            framebuffer_resized = false;
            return true;
        } else {
            return false;
        }
    }

    inline void set_key_callback(std::function<void(GLFWwindow*, int, int, int, int)> callback) noexcept
    {
        if (key_callback == nullptr)
            enable_key_callback();
        key_callback = callback;
    }
    
    inline void set_cursor_callback(std::function<void(GLFWwindow*, double, double)> callback) noexcept
    {
        if (cursor_callback == nullptr)
            enable_cursor_callback();
        cursor_callback = callback;
    }

    inline void set_mouse_button_callback(std::function<void(GLFWwindow*, int, int, int)> callback) noexcept
    {
        if (mouse_button_callback == nullptr)
            enable_mouse_button_callback();
        mouse_button_callback = callback;
    }

private:
    inline void get_surface(VkInstance instance, VkSurfaceKHR* surface)
    {
        VK_CHECK(glfwCreateWindowSurface(instance, m_window, nullptr, surface), "failed to create window surface");
    }

    void enable_key_callback();
    void enable_cursor_callback();
    void enable_mouse_button_callback();

    bool framebuffer_resized;
    GLFWwindow* m_window;

    std::function<void(GLFWwindow*, int, int, int, int)> key_callback = nullptr;
    std::function<void(GLFWwindow*, double, double)> cursor_callback = nullptr;
    std::function<void(GLFWwindow*, int, int, int)> mouse_button_callback = nullptr;
};

} // namespace quix

#endif // _QUIX_WINDOW_HPP