#ifndef _QUIX_PIPELINE_HPP
#define _QUIX_PIPELINE_HPP

#include "quix_instance.hpp"

#include "quix_pipeline_builder.hpp"

namespace quix {

namespace graphics {

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