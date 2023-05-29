#ifndef _QUIX_COMMAND_LIST_HPP
#define _QUIX_COMMAND_LIST_HPP

namespace quix {

    class instance;
    class device;

    class command_list {
    public:

    private:
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

        NODISCARD inline VkCommandPool get() const noexcept { return pool; }

    private:

        std::shared_ptr<device> m_device;
        VkCommandPool pool;
    };

} // namespace quix

#endif // _QUIX_COMMAND_LIST_HPP