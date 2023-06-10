#ifndef _QUIX_COMMAND_LIST_HPP
#define _QUIX_COMMAND_LIST_HPP

namespace quix {

class instance;
class device;
class swapchain;
class render_target;
namespace graphics {
    class pipeline;
}
class command_list;

class sync {
public:
    sync(weakref<device> p_device, weakref<swapchain> p_swapchain);
    ~sync();

    sync(const sync&) = delete;
    const sync& operator=(const sync&) = delete;
    sync(sync&&) = delete;
    const sync& operator=(sync&&) = delete;

    void wait_for_fence(const int frame);
    void reset_fence(const int frame);
    VkResult acquire_next_image(const int frame, uint32_t* image_index);
    VkResult submit_frame(const int frame, command_list* command);
    VkResult present_frame(const int frame, const uint32_t image_index);

private:
    void create_sync_objects();
    void destroy_sync_objects();

    weakref<device> m_device;
    weakref<swapchain> m_swapchain;

    int m_frames_in_flight;
    void* m_sync_buffer = nullptr;
    VkFence* m_fences = nullptr;
    VkSemaphore* m_available_semaphores = nullptr;
    VkSemaphore* m_finished_semaphores = nullptr;
};

class command_list {
public:
    command_list(weakref<device> p_device, VkCommandBuffer buffer);
    ~command_list() = default;

    command_list(const command_list&) = delete;
    command_list& operator=(const command_list&) = delete;
    command_list(command_list&&) = delete;
    command_list& operator=(command_list&&) = delete;

    NODISCARD inline VkCommandBuffer get_cmd_buffer() const noexcept { return buffer; }
    NODISCARD inline VkCommandBuffer* get_cmd_buffer_ref() { return &buffer; }

    void begin_record(VkCommandBufferUsageFlags flags = 0);
    void end_record();

    void begin_render_pass(const render_target& p_target, const std::shared_ptr<graphics::pipeline>& p_pipeline, uint32_t image_index, VkClearValue* clear_value, uint32_t clear_value_count);
    void end_render_pass();

    void copy_buffer_to_buffer(VkBuffer src_buffer, VkDeviceSize src_offset, VkBuffer dst_buffer, VkDeviceSize dst_offset, VkDeviceSize size);

private:
    weakref<device> m_device;
    VkCommandBuffer buffer;
};

class command_pool {
    friend class instance;

public:
    command_pool(weakref<device> p_device, VkCommandPool pool);
    ~command_pool();

    command_pool(const command_pool&) = delete;
    command_pool& operator=(const command_pool&) = delete;
    command_pool(command_pool&&) = delete;
    command_pool& operator=(command_pool&&) = delete;

    NODISCARD inline VkCommandPool get_pool() const noexcept { return pool; }

    NODISCARD allocated_unique_ptr<command_list> create_command_list(VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);

private:
    std::pmr::unsynchronized_pool_resource m_allocator;
    weakref<device> m_device;
    VkCommandPool pool;
};

} // namespace quix

#endif // _QUIX_COMMAND_LIST_HPP