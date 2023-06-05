#ifndef _QUIX_RESOURCE_CPP
#define _QUIX_RESOURCE_CPP

#include "quix_resource.hpp"

#include "quix_command_list.hpp"
#include "quix_device.hpp"
#include "quix_instance.hpp"

namespace quix {

buffer_handle::buffer_handle(std::shared_ptr<device> s_device)
    : m_device(std::move(s_device))
{
}

buffer_handle::~buffer_handle()
{
    vmaDestroyBuffer(m_device->get_allocator(), m_buffer, m_alloc);
}

void buffer_handle::create_buffer(const VkBufferCreateInfo* create_info, const VmaAllocationCreateInfo* alloc_info)
{
    VK_CHECK(vmaCreateBuffer(m_device->get_allocator(), create_info, alloc_info, &m_buffer, &m_alloc, &m_alloc_info), "failed to create buffer");
}

void buffer_handle::create_uniform_buffer(const VkDeviceSize size)
{
    VkBufferCreateInfo buffer_info {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = size,
        .usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
    };

    VmaAllocationCreateInfo alloc_info {
        .flags = VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT,
        .usage = VMA_MEMORY_USAGE_AUTO,
    };

    create_buffer(&buffer_info, &alloc_info);
}

void buffer_handle::create_staging_buffer(const VkDeviceSize size)
{
    VkBufferCreateInfo buffer_info {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = size,
        .usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
    };

    VmaAllocationCreateInfo alloc_info {
        .flags = VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT,
        .usage = VMA_MEMORY_USAGE_CPU_TO_GPU,
    };

    create_buffer(&buffer_info, &alloc_info);
}

void buffer_handle::create_staged_buffer(const VkDeviceSize size, const VkBufferUsageFlags usage_flags, const void* data, instance* inst)
{
    buffer_handle staging_buffer(m_device);
    staging_buffer.create_staging_buffer(size);
    memcpy(staging_buffer.get_mapped_data(), data, size);

    VkBufferCreateInfo buffer_info {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = size,
        .usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | usage_flags,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
    };

    VmaAllocationCreateInfo alloc_info {
        .usage = VMA_MEMORY_USAGE_GPU_ONLY,
    };

    create_buffer(&buffer_info, &alloc_info);

    auto cmd_pool = inst->get_command_pool();
    auto cmd_list = cmd_pool->create_command_list();

    cmd_list->begin_record(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

    cmd_list->copy_buffer_to_buffer(staging_buffer.get_buffer(), 0, m_buffer, 0, size);

    cmd_list->end_record();
}

} // namespace quix

#endif // _QUIX_RESOURCE_CPP