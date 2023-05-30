#ifndef _QUIX_RENDER_TARGET_CPP
#define _QUIX_RENDER_TARGET_CPP

#include "quix_render_target.hpp"

#include "quix_device.hpp"
#include "quix_swapchain.hpp"

namespace quix {

static constexpr renderpass_info<1, 1, 0> renderpass_info_default = {
    { VkAttachmentDescription {
        .format = VK_FORMAT_R8G8B8A8_SRGB,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR } },
    { VkAttachmentReference {
        .attachment = 0,
        .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL } },
    { VkSubpassDescription {
        .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .colorAttachmentCount = 1,
        .pColorAttachments = &renderpass_info_default.attachments_references[0] } },
    {}
};

render_target::render_target(std::shared_ptr<device> s_device, std::shared_ptr<swapchain> s_swapchain, const VkRenderPassCreateInfo* render_pass_create_info)
    : m_device(s_device), m_swapchain(s_swapchain)
{
    create_renderpass(render_pass_create_info);
    create_framebuffers();
}

render_target::~render_target() {
    for (auto framebuffer : m_framebuffers) {
        vkDestroyFramebuffer(m_device->get_logical_device(), framebuffer, nullptr);
    }

    vkDestroyRenderPass(m_device->get_logical_device(), m_render_pass, nullptr);
}

NODISCARD VkExtent2D render_target::get_extent() const noexcept
{
    return m_swapchain->get_extent();
}

void render_target::create_renderpass(const VkRenderPassCreateInfo* renderpass_info)
{
    if (vkCreateRenderPass(m_device->get_logical_device(), renderpass_info, nullptr, &m_render_pass) != VK_SUCCESS) {
        throw std::runtime_error("failed to create render pass!");
    }
}

void render_target::create_framebuffers()
{
    auto& swapchain_image_views = m_swapchain->get_image_views();

    m_framebuffers.resize(swapchain_image_views.size());
    for (size_t i = 0; i < swapchain_image_views.size(); i++) {
        VkImageView attachments[] = {
            swapchain_image_views[i]
        };

        VkFramebufferCreateInfo framebuffer_info {
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .renderPass = m_render_pass,
            .attachmentCount = 1,
            .pAttachments = attachments,
            .width = m_swapchain->get_extent().width,
            .height = m_swapchain->get_extent().height,
            .layers = 1
        };

        if (vkCreateFramebuffer(m_device->get_logical_device(), &framebuffer_info, nullptr, &m_framebuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }
}

} // namespace quix

#endif // _QUIX_RENDER_TARGET_CPP