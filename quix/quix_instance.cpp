#ifndef _QUIX_INSTANCE_CPP
#define _QUIX_INSTANCE_CPP

#include "quix_instance.hpp"

#include "quix_commands.hpp"
#include "quix_common.hpp"
#include "quix_descriptor.hpp"
#include "quix_device.hpp"
#include "quix_pipeline.hpp"
#include "quix_render_target.hpp"
#include "quix_resource.hpp"
#include "quix_swapchain.hpp"
#include "quix_window.hpp"

namespace quix {

consteval std::size_t get_allocation_size()
{
    return sizeof(window) + sizeof(device) + sizeof(swapchain) + sizeof(graphics::pipeline_manager) + sizeof(descriptor::allocator) + sizeof(descriptor::layout_cache);
}

instance::instance(const char* app_name,
    uint32_t app_version,
    int width,
    int height)
    : m_window(allocate_unique<window>(&m_allocator, app_name, width, height))
    , m_device(allocate_unique<device>(&m_allocator,
          make_weakref<window>(m_window),
          app_name,
          app_version,
          "quix",
          VK_MAKE_VERSION(1, 0, 0)))
    , m_swapchain(nullptr)
    , m_pipeline_manager(nullptr)
    , m_descriptor_allocator(nullptr)
    , m_descriptor_layout_cache(nullptr)
{
    static_assert(get_allocation_size()
            <= m_buffer_size,
        "instance buffer size is too small");
}

instance::~instance() = default;

void instance::create_device(std::vector<const char*>&& requested_extensions, VkPhysicalDeviceFeatures requested_features)
{
    m_device->init(std::move(requested_extensions), requested_features);

    m_descriptor_allocator = allocate_unique<descriptor::allocator>(&m_allocator, m_device->get_logical_device());
    m_descriptor_layout_cache = allocate_unique<descriptor::layout_cache>(&m_allocator, m_device->get_logical_device());
}

void instance::create_swapchain(const int32_t frames_in_flight, const VkPresentModeKHR present_mode, const bool depth_buffer)
{
    m_swapchain = allocate_unique<swapchain>(&m_allocator, make_weakref<instance>(this), make_weakref<window>(m_window), make_weakref<device>(m_device), frames_in_flight, present_mode, depth_buffer);
}

void instance::create_pipeline_manager()
{
    // TODO : make pipeline manager threadsafe (probably?)
    m_pipeline_manager = allocate_unique<graphics::pipeline_manager>(&m_allocator, make_weakref<device>(m_device));
}

NODISCARD command_pool instance::get_command_pool()
{
    return command_pool {
        make_weakref<device>(m_device),
        m_device->get_command_pool()
    };
}

NODISCARD render_target instance::create_single_pass_render_target() noexcept
{
    quix::renderpass_info<1, 1, 1> renderpass_info {};
    renderpass_info.attachments[0].format = get_surface_format().format;
    renderpass_info.attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
    renderpass_info.attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    renderpass_info.attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    renderpass_info.attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    renderpass_info.attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    renderpass_info.attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    renderpass_info.attachments[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    renderpass_info.attachment_references[0].attachment = 0;
    renderpass_info.attachment_references[0].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    renderpass_info.subpasses[0].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    renderpass_info.subpasses[0].colorAttachmentCount = 1;
    renderpass_info.subpasses[0].pColorAttachments = renderpass_info.attachment_references.data();

    renderpass_info.subpass_dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
    renderpass_info.subpass_dependencies[0].dstSubpass = 0;
    renderpass_info.subpass_dependencies[0].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    renderpass_info.subpass_dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    renderpass_info.subpass_dependencies[0].srcAccessMask = 0;
    renderpass_info.subpass_dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    renderpass_info.subpass_dependencies[0].dependencyFlags = 0;

    return create_render_target(renderpass_info.export_renderpass_info());
}

NODISCARD render_target instance::create_single_pass_depth_render_target() noexcept
{
    if (!m_swapchain->depth_buffer_enabled) {
        quix_error("cannot make a depth buffer render target when the depth buffer has been disabled");
    }

    quix::renderpass_info<2, 1, 1> renderpass_info {};
    renderpass_info.attachments[0].format = get_surface_format().format;
    renderpass_info.attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
    renderpass_info.attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    renderpass_info.attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    renderpass_info.attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    renderpass_info.attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    renderpass_info.attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    renderpass_info.attachments[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    renderpass_info.attachments[1].format = m_swapchain->find_depth_format();
    renderpass_info.attachments[1].samples = VK_SAMPLE_COUNT_1_BIT;
    renderpass_info.attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    renderpass_info.attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    renderpass_info.attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    renderpass_info.attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    renderpass_info.attachments[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    renderpass_info.attachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    renderpass_info.attachment_references[0].attachment = 0;
    renderpass_info.attachment_references[0].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    renderpass_info.attachment_references[1].attachment = 1;
    renderpass_info.attachment_references[1].layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    renderpass_info.subpasses[0].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    renderpass_info.subpasses[0].colorAttachmentCount = 1;
    renderpass_info.subpasses[0].pColorAttachments = &renderpass_info.attachment_references[0];
    renderpass_info.subpasses[0].pDepthStencilAttachment = &renderpass_info.attachment_references[1];

    renderpass_info.subpass_dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
    renderpass_info.subpass_dependencies[0].dstSubpass = 0;
    renderpass_info.subpass_dependencies[0].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    renderpass_info.subpass_dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    renderpass_info.subpass_dependencies[0].srcAccessMask = 0;
    renderpass_info.subpass_dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    renderpass_info.subpass_dependencies[0].dependencyFlags = 0;

    return create_render_target(renderpass_info.export_renderpass_info());
}

NODISCARD render_target instance::create_render_target(const VkRenderPassCreateInfo&& render_pass_create_info) noexcept
{
    return render_target {
        make_weakref<window>(m_window),
        make_weakref<device>(m_device),
        make_weakref<swapchain>(m_swapchain),
        &render_pass_create_info
    };
}

NODISCARD sync instance::create_sync_objects() noexcept
{
    return sync {
        make_weakref<device>(m_device),
        make_weakref<swapchain>(m_swapchain)
    };
}

NODISCARD buffer_handle instance::create_buffer_handle() const noexcept
{
    return buffer_handle {
        make_weakref<device>(m_device),
    };
}

NODISCARD image_handle instance::create_image_handle() const noexcept
{
    return image_handle {
        make_weakref<device>(m_device),
    };
}

void instance::wait_idle()
{
    m_device->wait_idle();
}

NODISCARD weakref<window>
instance::get_window() const noexcept
{
    return make_weakref<window>(m_window);
}

NODISCARD VkDevice
instance::get_logical_device() const noexcept
{
    return m_device->get_logical_device();
}

NODISCARD VkSurfaceFormatKHR instance::get_surface_format() const noexcept
{
    return m_swapchain->get_surface_format();
}

NODISCARD weakref<graphics::pipeline_manager> instance::get_pipeline_manager() noexcept
{
    if (m_pipeline_manager.get() == nullptr) {
        create_pipeline_manager();
    }
    return make_weakref<graphics::pipeline_manager>(m_pipeline_manager);
}

NODISCARD descriptor::allocator_pool instance::get_descriptor_allocator_pool() const noexcept
{
    return m_descriptor_allocator->getPool();
}

NODISCARD descriptor::builder instance::get_descriptor_builder(descriptor::allocator_pool* allocator_pool) const noexcept
{
    return descriptor::builder { m_descriptor_layout_cache.get(), allocator_pool };
}

NODISCARD VkFence instance::create_fence(VkFenceCreateFlags flags)
{
    VkFence fence = VK_NULL_HANDLE;

    VkFenceCreateInfo fence_info {};
    fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fence_info.flags = flags;

    VK_CHECK(vkCreateFence(m_device->get_logical_device(), &fence_info, nullptr, &fence), "failed to create fence");

    return fence;
}

NODISCARD weakref<device> instance::get_device() const noexcept
{
    return weakref<device> { m_device };
}

} // namespace quix

#endif // _QUIX_DEVICE_CPP
