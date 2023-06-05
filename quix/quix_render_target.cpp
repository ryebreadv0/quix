#ifndef _QUIX_RENDER_TARGET_CPP
#define _QUIX_RENDER_TARGET_CPP

#include "quix_render_target.hpp"

#include "quix_window.hpp"
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

render_target::render_target(std::shared_ptr<window> s_window,std::shared_ptr<device> s_device, std::shared_ptr<swapchain> s_swapchain, const VkRenderPassCreateInfo* render_pass_create_info)
    : m_window(std::move(s_window)), m_device(std::move(s_device)), m_swapchain(std::move(s_swapchain))
{
    create_renderpass(render_pass_create_info);
    create_framebuffers();
}

render_target::~render_target() {
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

        VK_CHECK(vkCreateFramebuffer(m_device->get_logical_device(), &framebuffer_info, nullptr, &m_framebuffers[i]), "failed to create framebuffer");
    }
}

void render_target::destroy_framebuffers()
{
    for (auto framebuffer : m_framebuffers) {
        vkDestroyFramebuffer(m_device->get_logical_device(), framebuffer, nullptr);
    }
}

} // namespace quix

#endif // _QUIX_RENDER_TARGET_CPP