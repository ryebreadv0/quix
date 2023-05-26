#ifndef _QUIX_PIPELINE_HPP
#define _QUIX_PIPELINE_HPP

#include "quix_instance.hpp"

#include "quix_pipeline_builder.hpp"

namespace quix {

namespace graphics {

    template <std::size_t Attachments, std::size_t Subpasses, std::size_t Dependencies>
    struct renderpass_info {
        VkAttachmentDescription attachments[Attachments];
        VkAttachmentReference attachments_references[Attachments];
        VkSubpassDescription subpasses[Subpasses];
        VkSubpassDependency subpass_dependencies[Dependencies];

        VkRenderPassCreateInfo create_renderpass_info() const noexcept
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

    

    class pipeline_manager {
    public:
        pipeline_manager();
        ~pipeline_manager();

        pipeline_manager(const pipeline_manager&) = delete;
        pipeline_manager& operator=(const pipeline_manager&) = delete;
        pipeline_manager(pipeline_manager&&) = delete;
        pipeline_manager& operator=(pipeline_manager&&) = delete;

    private:
        std::shared_ptr<device> s_device;
        std::shared_ptr<swapchain> s_swapchain;
    };

} // namespace graphics

} // namespace quix

#endif // _QUIX_PIPELINE_HPP