#ifndef _QUIX_PIPELINE_HPP
#define _QUIX_PIPELINE_HPP

#include "quix_pipeline_builder.hpp"

namespace quix {

class device;

namespace graphics {

    class pipeline_manager {
        friend class pipeline_builder;
    public:
        explicit pipeline_manager(std::shared_ptr<device> s_device);

        ~pipeline_manager() = default;

        pipeline_manager(const pipeline_manager&) = delete;
        pipeline_manager& operator=(const pipeline_manager&) = delete;
        pipeline_manager(pipeline_manager&&) = delete;
        pipeline_manager& operator=(pipeline_manager&&) = delete;

        std::shared_ptr<pipeline_builder> create_pipeline_builder(std::shared_ptr<render_target> render_target);

    private:
        std::pmr::monotonic_buffer_resource m_allocator;
        std::shared_ptr<device> m_device;
    };

} // namespace graphics

} // namespace quix

#endif // _QUIX_PIPELINE_HPP