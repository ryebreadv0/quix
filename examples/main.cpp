#include "quix_descriptor.hpp"
#include "quix_instance.hpp"
#include "quix_pipeline.hpp"

static constexpr int WIDTH = 800;
static constexpr int HEIGHT = 600;

int main()
{
    quix::instance instance("quix_example",
        VK_MAKE_VERSION(0, 0, 1),
        WIDTH, HEIGHT);

    instance.create_device({ VK_KHR_SWAPCHAIN_EXTENSION_NAME },
        { .tessellationShader = VK_TRUE });

    instance.create_swapchain(2, VK_PRESENT_MODE_FIFO_KHR);

    auto device = instance.get_logical_device();

    quix::graphics::renderpass_info<1, 1, 0> renderpass_info = {
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
        {}
    };

    quix::graphics::pipeline_builder pipeline_builder(device);

    auto shader_stages = pipeline_builder.create_shader_array(
        pipeline_builder.create_shader_stage("examples/simpleshader.vert", VK_SHADER_STAGE_VERTEX_BIT),
        pipeline_builder.create_shader_stage("examples/simpleshader.frag", VK_SHADER_STAGE_FRAGMENT_BIT));

    auto allocator_pool = instance.get_descriptor_allocator_pool();
    auto descriptor_set_builder = instance.get_descriptor_builder(&allocator_pool);
    descriptor_set_builder.bind_buffer(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
    auto descriptor_set_layout = descriptor_set_builder.buildLayout();

    auto pipeline = pipeline_builder.add_shader_stages(shader_stages)
        .add_push_constant(VK_SHADER_STAGE_VERTEX_BIT, sizeof(float) * 4)
        .add_descriptor_set_layout(descriptor_set_layout)
        .add_renderpass_info(renderpass_info.create_renderpass_info())
        .create_graphics_pipeline();

    auto* window = instance.window();

    while (glfwWindowShouldClose(window) == GLFW_FALSE) {
        glfwPollEvents();
    }

    return 0;
}