#include "quix_command_list.hpp"
#include "quix_descriptor.hpp"
#include "quix_instance.hpp"
#include "quix_pipeline.hpp"
#include "quix_render_target.hpp"

static constexpr int WIDTH = 800;
static constexpr int HEIGHT = 600;
static constexpr int FRAMES_IN_FLIGHT = 2;

int main()
{
    quix::instance instance("quix_example",
        VK_MAKE_VERSION(0, 0, 1),
        WIDTH, HEIGHT);

    instance.create_device({ VK_KHR_SWAPCHAIN_EXTENSION_NAME },
        {});

    instance.create_swapchain(FRAMES_IN_FLIGHT, VK_PRESENT_MODE_FIFO_KHR);

    quix::renderpass_info<1, 1, 1> renderpass_info = {
        { VkAttachmentDescription {
            .format = instance.get_surface_format().format,
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
            .pColorAttachments = &renderpass_info.attachments_references[0] } },
        { VkSubpassDependency {
            VK_SUBPASS_EXTERNAL,
            0,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            0,
            VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            0 } }
    };

    auto render_target = instance.create_render_target(renderpass_info.export_renderpass_info());

    instance.create_pipeline_manager();
    auto pipeline_manager = instance.get_pipeline_manager();
    auto pipeline_builder = pipeline_manager->create_pipeline_builder(render_target);

    auto shader_stages = pipeline_builder->create_shader_array(
        pipeline_builder->create_shader_stage("examples/simpleshader.vert", VK_SHADER_STAGE_VERTEX_BIT),
        pipeline_builder->create_shader_stage("examples/simpleshader.frag", VK_SHADER_STAGE_FRAGMENT_BIT));

    auto allocator_pool = instance.get_descriptor_allocator_pool();
    auto descriptor_set_builder = instance.get_descriptor_builder(&allocator_pool);
    auto descriptor_set_layout = descriptor_set_builder
                                     .bind_buffer(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
                                     .buildLayout();

    auto pipeline = pipeline_builder->add_shader_stages(shader_stages)
                        .add_push_constant(VK_SHADER_STAGE_VERTEX_BIT, sizeof(float) * 4)
                        .add_descriptor_set_layout(descriptor_set_layout)
                        .create_graphics_pipeline();

    auto command_pool = instance.get_command_pool();

    auto* window = instance.window();

    int current_frame = 0;
    uint32_t current_image_index = 0;

    auto sync_objects = instance.create_sync_objects();

    std::array<std::shared_ptr<quix::command_list>, FRAMES_IN_FLIGHT> command_lists {
        command_pool->create_command_list(), command_pool->create_command_list()
    };

    std::array<VkClearValue, 1> clear_values = {
        { { 0.0f, 0.0f, 0.0f, 0.0f } }
    };

    while (glfwWindowShouldClose(window) == GLFW_FALSE) {
        glfwPollEvents();

        VkResult result = sync_objects->acquire_next_image(current_frame, &current_image_index);
        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            render_target->recreate_swapchain();
            continue;
        } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            quix_error("failed to acquire swapchain image");
        }
        sync_objects->reset_fence(current_frame);

        command_lists[current_frame]->begin_record();

        command_lists[current_frame]->begin_render_pass(render_target, pipeline, current_image_index, clear_values.data(), clear_values.size());

        vkCmdDraw(command_lists[current_frame]->get_buffer(), 3, 1, 0, 0);

        command_lists[current_frame]->end_render_pass();

        command_lists[current_frame]->end_record();

        VK_CHECK(sync_objects->submit_frame(current_frame, command_lists[current_frame]), "failed to submit frame");

        result = sync_objects->present_frame(current_frame, current_image_index);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
            render_target->recreate_swapchain();
        else if (result != VK_SUCCESS)
            quix_error("failed to present swapchain image");

        current_frame = (current_frame + 1) % FRAMES_IN_FLIGHT;
    }

    instance.wait_idle();

    return 0;
}