#ifndef _QUIX_RENDER_TARGET_HPP
#define _QUIX_RENDER_TARGET_HPP

namespace quix {

class window;
class device;
class swapchain;

template <std::size_t Attachments, std::size_t Subpasses, std::size_t Dependencies>
struct renderpass_info {
    std::array<VkAttachmentDescription, Attachments> attachments{};
    std::array<VkAttachmentReference, Attachments> attachment_references{};
    std::array<VkSubpassDescription, Subpasses> subpasses{};
    std::array<VkSubpassDependency, Dependencies> subpass_dependencies{};

    NODISCARD VkRenderPassCreateInfo export_renderpass_info() const noexcept
    {
        return VkRenderPassCreateInfo {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
            .attachmentCount = Attachments,
            .pAttachments = attachments.data(),
            .subpassCount = Subpasses,
            .pSubpasses = subpasses.data(),
            .dependencyCount = Dependencies,
            .pDependencies = subpass_dependencies.data()
        };
    }
};

// TODO : check and see if I need to do anything about frames in flight
class render_target {
public:
    render_target(weakref<window> p_window, weakref<device> p_device, weakref<swapchain> p_swapchain, const VkRenderPassCreateInfo* render_pass_create_info);
    ~render_target();

    render_target(const render_target&) = delete;
    render_target& operator=(const render_target&) = delete;
    render_target(render_target&&) = delete;
    render_target& operator=(render_target&&) = delete;

    NODISCARD inline VkRenderPass get_render_pass() const noexcept { return m_render_pass; }
    NODISCARD inline VkFramebuffer get_framebuffer(uint32_t index) const noexcept { return m_framebuffers[index]; }
    NODISCARD VkExtent2D get_extent() const noexcept;

    void recreate_swapchain();

private:
    void create_renderpass(const VkRenderPassCreateInfo* renderpass_info);
    void create_framebuffers();
    void destroy_framebuffers();

    weakref<window> m_window;
    weakref<device> m_device;
    weakref<swapchain> m_swapchain;

    std::vector<VkFramebuffer> m_framebuffers;
    VkRenderPass m_render_pass = VK_NULL_HANDLE;
};

} // namespace quix

#endif // _QUIX_RENDER_TARGET_HPP