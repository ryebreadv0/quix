#ifndef _QUIX_RESOURCE_HPP
#define _QUIX_RESOURCE_HPP

namespace quix {

class device;
class instance;

class buffer_handle {
public:
    explicit buffer_handle(std::shared_ptr<device> s_device);

    ~buffer_handle();
    buffer_handle(const buffer_handle&) = delete;
    buffer_handle& operator=(const buffer_handle&) = delete;
    buffer_handle(buffer_handle&&) = delete;
    buffer_handle& operator=(buffer_handle&&) = delete;

    void create_buffer(const VkBufferCreateInfo* create_info, const VmaAllocationCreateInfo* alloc_info);
    void create_uniform_buffer(const VkDeviceSize size);
    void create_gpu_buffer(const VkDeviceSize size, const VkBufferUsageFlags usage_flags);
    void create_staging_buffer(const VkDeviceSize size);
    void create_staged_buffer(const VkDeviceSize size, const VkBufferUsageFlags usage_flags, const void* data, instance* inst);

    NODISCARD inline VkBuffer get_buffer() const noexcept { return m_buffer; }
    NODISCARD inline VmaAllocationInfo get_alloc_info() const noexcept { return m_alloc_info; }
    NODISCARD inline void* get_mapped_data() const noexcept
    {
        quix_assert(m_alloc_info.pMappedData != nullptr, "buffer is not mapped");
        return m_alloc_info.pMappedData;
    }

private:
    std::shared_ptr<device> m_device;

    VmaAllocation m_alloc {};
    VmaAllocationInfo m_alloc_info {};
    VkBuffer m_buffer = VK_NULL_HANDLE;
};

} // namespace quix

#endif // _QUIX_RESOURCE_HPP