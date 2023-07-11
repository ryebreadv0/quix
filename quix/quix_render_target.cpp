#ifndef _QUIX_RENDER_TARGET_CPP
#define _QUIX_RENDER_TARGET_CPP

#include "quix_render_target.hpp"

#include "quix_device.hpp"
#include "quix_swapchain.hpp"
#include "quix_window.hpp"
#include "quix_resource.hpp"

namespace quix {

// consteval this shit

[[maybe_unused]] static consteval renderpass_info<1, 1, 1> get_default_renderpass_info()
{
    quix::renderpass_info<1, 1, 1> info {};
    info.attachments[0].format = VK_FORMAT_B8G8R8A8_SRGB;
    info.attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
    info.attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    info.attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    info.attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    info.attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    info.attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    info.attachments[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    info.attachment_references[0].attachment = 0;
    info.attachment_references[0].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    info.subpasses[0].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    info.subpasses[0].colorAttachmentCount = 1;
    info.subpasses[0].pColorAttachments = info.attachment_references.data();

    info.subpass_dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
    info.subpass_dependencies[0].dstSubpass = 0;
    info.subpass_dependencies[0].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    info.subpass_dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    info.subpass_dependencies[0].srcAccessMask = 0;
    info.subpass_dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    info.subpass_dependencies[0].dependencyFlags = 0;

    return info;
}

render_target::render_target(weakref<window> p_window, weakref<device> p_device, weakref<swapchain> p_swapchain, const VkRenderPassCreateInfo* render_pass_create_info)
    : m_window(std::move(p_window))
    , m_device(std::move(p_device))
    , m_swapchain(std::move(p_swapchain))
{
    create_renderpass(render_pass_create_info);
    create_framebuffers();
}

render_target::~render_target()
{
    destroy_framebuffers();

    vkDestroyRenderPass(m_device->get_logical_device(), m_render_pass, nullptr);
}

NODISCARD VkExtent2D render_target::get_extent() const noexcept
{
    return m_swapchain->get_extent();
}

void render_target::recreate_swapchain()
{
    auto* window = m_window->get_window();
    int width = 0;
    int height = 0;
    while (width == 0 && height == 0) {
        glfwGetFramebufferSize(window, &width, &height);
        glfwWaitEvents();
    }

    m_device->wait_idle();

    m_swapchain->recreate_swapchain();

    destroy_framebuffers();

    create_framebuffers();
}

void render_target::create_renderpass(const VkRenderPassCreateInfo* renderpass_info)
{
    VK_CHECK(vkCreateRenderPass(m_device->get_logical_device(), renderpass_info, nullptr, &m_render_pass), "failed to create renderpass");
}

void render_target::create_framebuffers()
{
    const auto& swapchain_image_views = m_swapchain->get_image_views();
    auto *depth_view = m_swapchain->depth_image->get_view();
    uint32_t attachment_count;
    if (depth_view != VK_NULL_HANDLE) {
        attachment_count = 2;
    } else {
        attachment_count = 1;
    }

    m_framebuffers.resize(swapchain_image_views.size());
    for (size_t i = 0; i < swapchain_image_views.size(); i++) {
        VkImageView attachments[2] = {
            swapchain_image_views[i],
            depth_view
        };

        VkFramebufferCreateInfo framebuffer_info {};
        framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebuffer_info.renderPass = m_render_pass;
        framebuffer_info.attachmentCount = attachment_count;
        framebuffer_info.pAttachments = attachments;
        framebuffer_info.width = m_swapchain->get_extent().width;
        framebuffer_info.height = m_swapchain->get_extent().height;
        framebuffer_info.layers = 1;

        VK_CHECK(vkCreateFramebuffer(m_device->get_logical_device(), &framebuffer_info, nullptr, &m_framebuffers[i]), "failed to create framebuffer");
    }
}

void render_target::destroy_framebuffers()
{
    for (auto* framebuffer : m_framebuffers) {
        vkDestroyFramebuffer(m_device->get_logical_device(), framebuffer, nullptr);
    }
}

} // namespace quix

#endif // _QUIX_RENDER_TARGET_CPP
