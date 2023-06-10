#ifndef _QUIX_DESCRIPTOR_HPP
#define _QUIX_DESCRIPTOR_HPP

namespace quix {

namespace descriptor {

    class allocator;

    struct allocator_pool {
        friend class allocator;
        friend class builder;
    public:
        allocator_pool(allocator* p_allocator, VkDescriptorPool pool);
        ~allocator_pool();

        allocator_pool(const allocator_pool&) = delete;
        const allocator_pool& operator=(allocator_pool const&) = delete;
        allocator_pool(allocator_pool&& other) = delete;
        allocator_pool& operator=(allocator_pool&& other) = delete;

        void returnPool();
    private:
        VkDescriptorSet allocate(VkDescriptorSetLayout layout);

        allocator* m_allocator { nullptr };
        std::deque<VkDescriptorPool> usedPools;
        VkDescriptorPool currentPool { VK_NULL_HANDLE };
    };

    class allocator {
        friend struct allocator_pool;
    public:
        explicit allocator(VkDevice device);

        ~allocator();
        allocator(const allocator&) = delete;
        allocator& operator=(const allocator&) = delete;
        allocator(allocator&&) = delete;
        allocator& operator=(allocator&&) = delete;

        void cleanup();
        VkDevice getDevice();
        allocator_pool getPool();

    private:

        VkDevice device { VK_NULL_HANDLE };

        VkDescriptorPool createDescriptorPool(int count, VkDescriptorPoolCreateFlags flags);

        std::mutex poolMutex;
        std::deque<VkDescriptorPool> availablePools;

        VkDescriptorPool borrowPool();
        void returnPool(allocator_pool& pool);
    };

    class layout_cache {
    public:
        layout_cache(VkDevice device);
        void cleanup();

        ~layout_cache();
        layout_cache(const layout_cache&) = delete;
        layout_cache& operator=(const layout_cache&) = delete;
        layout_cache(layout_cache&&) = delete;
        layout_cache& operator=(layout_cache&&) = delete;

        // unordered map is sychronized
        VkDescriptorSetLayout create_descriptor_layout(VkDescriptorSetLayoutCreateInfo* info);

        struct descriptor_layout_info {
            // good idea to turn this into an inlined array
            std::vector<VkDescriptorSetLayoutBinding> bindings;

            bool operator==(const descriptor_layout_info& other) const;

            size_t hash() const;
        };

    private:
        struct descriptor_layout_hash {
            std::size_t operator()(const descriptor_layout_info& k) const
            {
                return k.hash();
            }
        };

        std::mutex layoutCacheMutex;
        std::unordered_map<descriptor_layout_info, VkDescriptorSetLayout, descriptor_layout_hash> layoutCache;
        VkDevice device;
    };

    class builder {
    public:
        // static builder begin(layout_cache* layoutCache, allocator_pool* pool);
        builder(layout_cache* layoutCache, allocator_pool* pool);

        builder& bind_buffer(uint32_t binding, VkDescriptorType type, VkShaderStageFlags stageFlags);
        builder& bind_image(uint32_t binding, VkDescriptorType type, VkShaderStageFlags stageFlags);

        builder& update_buffer(uint32_t binding, VkDescriptorBufferInfo* bufferInfo);
        builder& update_image(uint32_t binding, VkDescriptorImageInfo* imageInfo);

        VkDescriptorSetLayout buildLayout();

        VkDescriptorSet buildSet();

    private:
        std::vector<VkWriteDescriptorSet> writes;
        std::vector<VkDescriptorSetLayoutBinding> bindings;

        VkDescriptorSetLayout layout;

        layout_cache* cache;
        allocator_pool* alloc;
    };

} // namespace descriptor

} // namespace quix

#endif // _QUIX_DESCRIPTOR_HPP
