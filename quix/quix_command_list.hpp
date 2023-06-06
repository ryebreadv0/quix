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
    sync(std::shared_ptr<device> s_device, std::shared_ptr<swapchain> s_swapchain);
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

    std::shared_ptr<device> m_device;
    std::shared_ptr<swapchain> m_swapchain;

    int m_frames_in_flight;
    void* m_sync_buffer = nullptr;
    VkFence* m_fences = nullptr;
    VkSemaphore* m_available_semaphores = nullptr;
    VkSemaphore* m_finished_semaphores = nullptr;
};

class command_list {
public:
    command_list(std::shared_ptr<device> s_device, VkCommandBuffer buffer);
    ~command_list() = default;

    command_list(const command_list&) = delete;
    command_list& operator=(const command_list&) = delete;
    command_list(command_list&&) = delete;
    command_list& operator=(command_list&&) = delete;

    NODISCARD inline VkCommandBuffer get_cmd_buffer() const noexcept { return buffer; }
    NODISCARD inline VkCommandBuffer* get_cmd_buffer_ref() { return &buffer; }

    void begin_record(VkCommandBufferUsageFlags flags = 0);
    void end_record();

    void begin_render_pass(const std::shared_ptr<render_target>& target, const std::shared_ptr<graphics::pipeline>& pipeline, uint32_t image_index, VkClearValue* clear_value, uint32_t clear_value_count);
    void end_render_pass();

    void copy_buffer_to_buffer(VkBuffer src_buffer, VkDeviceSize src_offset, VkBuffer dst_buffer, VkDeviceSize dst_offset, VkDeviceSize size);

private:
    VkCommandBuffer buffer;
    std::shared_ptr<device> m_device;
};

class command_pool {
    friend class instance;

public:
    command_pool(std::shared_ptr<device> s_device, VkCommandPool pool);
    ~command_pool();

    command_pool(const command_pool&) = delete;
    command_pool& operator=(const command_pool&) = delete;
    command_pool(command_pool&&) = delete;
    command_pool& operator=(command_pool&&) = delete;

    NODISCARD inline VkCommandPool get_pool() const noexcept { return pool; }

    NODISCARD allocated_unique_ptr<command_list> create_command_list(VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);

private:
    std::pmr::unsynchronized_pool_resource m_allocator;
    std::shared_ptr<device> m_device;
    VkCommandPool pool;
};

} // namespace quix

#endif // _QUIX_COMMAND_LIST_HPP