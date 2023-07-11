#ifndef _QUIX_RESOURCE_HPP
#define _QUIX_RESOURCE_HPP

namespace quix {

class device;
class instance;
class command_list;

class buffer_handle {
public:
    explicit buffer_handle(weakref<device> p_device);

    ~buffer_handle();
    buffer_handle(const buffer_handle&) = delete;
    buffer_handle& operator=(const buffer_handle&) = delete;
    buffer_handle(buffer_handle&&) = delete;
    buffer_handle& operator=(buffer_handle&&) = delete;

    void create_buffer(const VkBufferCreateInfo* create_info, const VmaAllocationCreateInfo* alloc_info);
    void create_uniform_buffer(const VkDeviceSize size);
    void create_staging_buffer(const VkDeviceSize size);
    void create_cpu_buffer(const VkDeviceSize size, const VkBufferUsageFlags usage_flags, const VmaAllocationCreateFlags alloc_flags);
    void create_gpu_buffer(const VkDeviceSize size, const VkBufferUsageFlags usage_flags);
    void create_staged_buffer(const VkDeviceSize size, const VkBufferUsageFlags usage_flags, const void* data, instance* inst);

    NODISCARD inline VkBuffer get_buffer() const noexcept { return m_buffer; }
    NODISCARD inline VmaAllocationInfo get_alloc_info() const noexcept { return m_alloc_info; }
    NODISCARD inline void* get_mapped_data() const noexcept
    {
        quix_assert(m_alloc_info.pMappedData != nullptr, "buffer is not mapped");
        return m_alloc_info.pMappedData;
    }
    NODISCARD inline VkDeviceSize get_offset() const noexcept { return m_alloc_info.offset; }
    NODISCARD inline VkDescriptorBufferInfo get_descriptor_info(uint32_t offset = 0)
    {
        VkDescriptorBufferInfo info{};
        info.buffer = m_buffer;
        info.offset = offset;
        info.range = m_alloc_info.size;
        return info;
    }

private:

    weakref<device> m_device;
    VmaAllocation m_alloc {};
    VmaAllocationInfo m_alloc_info {};
    VkBuffer m_buffer = VK_NULL_HANDLE;
};

class image_handle {
friend class command_list;
public:
    explicit image_handle(weakref<device> p_device);

    ~image_handle();
    image_handle(const image_handle&) = delete;
    image_handle& operator=(const image_handle&) = delete;
    image_handle(image_handle&&) = delete;
    image_handle& operator=(image_handle&&) = delete;

    void create_image(const VkImageCreateInfo* create_info, const VmaAllocationCreateInfo* alloc_info);

    image_handle& create_image_from_file(const char* filepath, instance* inst);
    image_handle& create_depth_image(uint32_t width, uint32_t height, VkFormat format);

    image_handle& create_view(VkImageAspectFlags aspect_flags = VK_IMAGE_ASPECT_COLOR_BIT);
    image_handle& create_sampler(VkFilter m_filter, VkSamplerAddressMode sampler_address_mode);
    image_handle& create_sampler(VkFilter m_filter, VkSamplerAddressMode sampler_address_mode, float anisotropy);

    NODISCARD inline VkImage get_image() const noexcept { return m_image; }
    NODISCARD inline VkImageView get_view() const noexcept { return m_view; }
    NODISCARD inline VkSampler get_sampler() const noexcept { return m_sampler; }

    NODISCARD inline VkDescriptorImageInfo get_descriptor_info()
    {
        VkDescriptorImageInfo info{};
        info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        info.imageView = m_view;
        info.sampler = m_sampler;
        return info;
    }

    void destroy_image();

private:

    constexpr VkImageViewType type_to_view_type();

    weakref<device> m_device;
    VmaAllocation m_alloc {};
    VmaAllocationInfo m_alloc_info {};
    VkImage m_image = VK_NULL_HANDLE;
    VkImageView m_view = VK_NULL_HANDLE;
    VkSampler m_sampler = VK_NULL_HANDLE;

    VkImageType m_type{};
    VkFormat m_format{};
    uint32_t m_mip_levels{};
    uint32_t m_array_layers{};
    VkSampleCountFlagBits m_samples{};
    VkExtent3D m_extent{};

};

} // namespace quix

#endif // _QUIX_RESOURCE_HPP
