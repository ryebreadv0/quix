#ifndef _QUIX_RENDER_TARGET_HPP
#define _QUIX_RENDER_TARGET_HPP

namespace quix {

class device;
class swapchain;

template <std::size_t Attachments, std::size_t Subpasses, std::size_t Dependencies>
struct renderpass_info {
    VkAttachmentDescription attachments[Attachments];
    VkAttachmentReference attachments_references[Attachments];
    VkSubpassDescription subpasses[Subpasses];
    VkSubpassDependency subpass_dependencies[Dependencies];

    NODISCARD VkRenderPassCreateInfo export_renderpass_info() const noexcept
    {
        return VkRenderPassCreateInfo {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
            .attachmentCount = Attachments,
            .pAttachments = attachments,
            .subpassCount = Subpasses,
            .pSubpasses = subpasses,
            .dependencyCount = Dependencies,
            .pDependencies = subpass_dependencies
        };
    }
};

// TODO : check and see if I need to do anything about frames in flight
class render_target {
public:
    render_target(std::shared_ptr<device> s_device, std::shared_ptr<swapchain> s_swapchain, const VkRenderPassCreateInfo* render_pass_create_info);

    ~render_target();

    render_target(const render_target&) = delete;
    render_target& operator=(const render_target&) = delete;
    render_target(render_target&&) = delete;
    render_target& operator=(render_target&&) = delete;

    NODISCARD VkRenderPass get_render_pass() const noexcept { return m_render_pass; }

private:
    void create_renderpass(const VkRenderPassCreateInfo* renderpass_info);
    void create_framebuffers();

    std::shared_ptr<device> m_device;
    std::shared_ptr<swapchain> m_swapchain;

    std::vector<VkFramebuffer> m_framebuffers;
    VkRenderPass m_render_pass;
};

} // namespace quix

#endif // _QUIX_RENDER_TARGET_HPP