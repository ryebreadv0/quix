#include "quix_commands.hpp"
#include "quix_common.hpp"
#include "quix_descriptor.hpp"
#include "quix_instance.hpp"
#include "quix_pipeline.hpp"
#include "quix_render_target.hpp"
#include "quix_resource.hpp"
#include "quix_window.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

static constexpr int WIDTH = 800;
static constexpr int HEIGHT = 600;
static constexpr int FRAMES_IN_FLIGHT = 2;

struct Vertex {
    glm::vec3 pos;
    glm::vec3 color;
    glm::vec2 tex;

    static constexpr std::array<VkVertexInputBindingDescription, 1> get_binding_description()
    {
        return {
            VkVertexInputBindingDescription {
                .binding = 0,
                .stride = sizeof(Vertex),
                .inputRate = VK_VERTEX_INPUT_RATE_VERTEX }
        };
    }

    static constexpr std::array<VkVertexInputAttributeDescription, 3> get_attribute_description()
    {
        return {
            VkVertexInputAttributeDescription {
                .location = 0,
                .binding = 0,
                .format = VK_FORMAT_R32G32B32_SFLOAT,
                .offset = offsetof(Vertex, pos) },
            VkVertexInputAttributeDescription {
                .location = 1,
                .binding = 0,
                .format = VK_FORMAT_R32G32B32_SFLOAT,
                .offset = offsetof(Vertex, color) },

            VkVertexInputAttributeDescription {
                .location = 2,
                .binding = 0,
                .format = VK_FORMAT_R32G32_SFLOAT,
                .offset = offsetof(Vertex, tex) }
        };
    }
};

struct uniform_buffer_object {
    alignas(16) glm::mat4 model;
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
    uniform_buffer_object()
        : model(glm::mat4(1.0f))
        , view(glm::mat4(1.0f))
        , proj(glm::mat4(1.0f))
    {
        // model = glm::rotate(glm::mat4(1.0f), 1.0f * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        proj = glm::perspective(glm::radians(90.0f), 800.0f / 600.0f, 0.1f, 10.0f);
        proj[1][1] *= -1;
    }

    void update(float timestep)
    {
        model = glm::rotate(glm::mat4(1.0f), glm::radians(glm::radians(90.0f)) * timestep * 100.0f, glm::vec3(0.0f, 1.0f, 0.0f));
    }
};

int main()
{
    quix::instance instance("quix_example",
        VK_MAKE_VERSION(0, 0, 1),
        WIDTH, HEIGHT);

    instance.create_device({ VK_KHR_SWAPCHAIN_EXTENSION_NAME },
        {});
    instance.create_swapchain(FRAMES_IN_FLIGHT, VK_PRESENT_MODE_FIFO_KHR, true);

    auto vertices = quix::create_auto_array<Vertex>(
        Vertex { glm::vec3 { -0.5f, -0.5f, 0.0f }, glm::vec3 { 1.0f, 0.0f, 0.0f }, glm::vec2 { 0.0f, 0.0f } },
        Vertex { glm::vec3 { 0.5f, -0.5f, 0.0f }, glm::vec3 { 0.0f, 1.0f, 0.0f }, glm::vec2 { 1.0f, 0.0f } },
        Vertex { glm::vec3 { 0.5f, 0.5f, 0.0f }, glm::vec3 { 0.0f, 0.0f, 1.0f }, glm::vec2 { 1.0f, 1.0f } },
        Vertex { glm::vec3 { -0.5f, 0.5f, 0.0f }, glm::vec3 { 1.0f, 1.0f, 1.0f }, glm::vec2 { 0.0f, 1.0f } },

        Vertex { glm::vec3 { -0.5f, -0.5f, -0.5f }, glm::vec3 { 1.0f, 0.0f, 0.0f }, glm::vec2 { 0.0f, 0.0f } },
        Vertex { glm::vec3 { 0.5f, -0.5f, -0.5f }, glm::vec3 { 0.0f, 1.0f, 0.0f }, glm::vec2 { 1.0f, 0.0f } },
        Vertex { glm::vec3 { 0.5f, 0.5f, -0.5f }, glm::vec3 { 0.0f, 0.0f, 1.0f }, glm::vec2 { 1.0f, 1.0f } },
        Vertex { glm::vec3 { -0.5f, 0.5f, -0.5f }, glm::vec3 { 1.0f, 1.0f, 1.0f }, glm::vec2 { 0.0f, 1.0f } });

    auto indices = quix::create_auto_array<uint16_t>(
        0, 1, 2, 2, 3, 0,
        4, 5, 6, 6, 7, 4);

    auto vertex_buffer = instance.create_buffer_handle();
    vertex_buffer.create_staged_buffer(sizeof(Vertex) * vertices.size(),
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        vertices.data(),
        &instance);

    auto index_buffer = instance.create_buffer_handle();
    index_buffer.create_staged_buffer(sizeof(uint16_t) * indices.size(),
        VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
        indices.data(),
        &instance);

    auto image = instance.create_image_handle();
    image.create_image_from_file("examples/img.jpg", &instance)
        .create_view()
        .create_sampler(VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_REPEAT);

    auto uniform_buffers = std::array<quix::buffer_handle, FRAMES_IN_FLIGHT> {
        instance.create_buffer_handle(), instance.create_buffer_handle()
    };
    for (auto& uniform_buffer : uniform_buffers) {
        uniform_buffer.create_uniform_buffer(sizeof(uniform_buffer_object));
        *((uniform_buffer_object*)uniform_buffer.get_mapped_data()) = uniform_buffer_object {};
    }

    auto render_target = instance.create_single_pass_depth_render_target();

    auto pipeline_manager = instance.get_pipeline_manager();
    auto pipeline_builder = pipeline_manager->create_pipeline_builder(&render_target);

    auto vertex_binding_description = Vertex::get_binding_description();
    auto vertex_attribute_description = Vertex::get_attribute_description();

    auto shader_stages = pipeline_builder.create_shader_array(
        pipeline_builder.create_shader_stage("examples/simpleshader.vert", VK_SHADER_STAGE_VERTEX_BIT),
        pipeline_builder.create_shader_stage("examples/simpleshader.frag", VK_SHADER_STAGE_FRAGMENT_BIT));

    auto allocator_pool = instance.get_descriptor_allocator_pool();
    auto descriptor_set_builder = instance.get_descriptor_builder(&allocator_pool);
    auto descriptor_set_layout = descriptor_set_builder
                                     .bind_buffer(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
                                     .bind_image(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
                                     .buildLayout();

    std::array<VkDescriptorSet, FRAMES_IN_FLIGHT> descriptor_sets {};
    {
        auto image_info = image.get_descriptor_info();
        descriptor_set_builder.update_image(1, &image_info);
        for (uint32_t index = 0; index < FRAMES_IN_FLIGHT; index++) {
            auto buffer_info = uniform_buffers[index].get_descriptor_info();
            descriptor_set_builder.update_buffer(0, &buffer_info);
            descriptor_sets[index] = descriptor_set_builder.buildSet();
        }
    }

    auto pipeline = pipeline_builder.add_shader_stages(shader_stages)
                        .create_vertex_state(vertex_binding_description.data(), vertex_binding_description.size(), vertex_attribute_description.data(), vertex_attribute_description.size())
                        .create_rasterization_state(VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE, VK_FRONT_FACE_CLOCKWISE)
                        // .add_push_constant(VK_SHADER_STAGE_VERTEX_BIT, sizeof(float) * 4)
                        .add_descriptor_set_layout(descriptor_set_layout)
                        .create_depth_stencil_state(VK_TRUE, VK_TRUE, VK_COMPARE_OP_LESS)
                        .create_graphics_pipeline();

    auto window = instance.get_window();
    auto command_pool = instance.get_command_pool();
    auto sync_objects = instance.create_sync_objects();

    int current_frame = 0;
    uint32_t current_image_index = 0;

    std::array<quix::allocated_unique_ptr<quix::command_list>, FRAMES_IN_FLIGHT> command_lists {
        command_pool.create_command_list(), command_pool.create_command_list()
    };

    std::array<VkClearValue, 2> clear_values = {
        { { 0.0f, 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f } }
    };

    std::array<VkBuffer, 1> vertex_buffer_array = { vertex_buffer.get_buffer() };
    std::array<VkDeviceSize, 1> offsets = { 0 };

    std::chrono::high_resolution_clock clock;
    auto start_time = clock.now();

    uniform_buffer_object uniform_buffer_main {};
    auto update_uniform = [&]() {
        auto cur_time = clock.now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>(cur_time - start_time).count();
        uniform_buffer_main.update(time);
        memcpy(uniform_buffers[current_frame].get_mapped_data(), &uniform_buffer_main, sizeof(uniform_buffer_main));
    };

    while (!window->should_close()) {
        window->poll_events();

        VkResult result = sync_objects.acquire_next_image(current_frame, &current_image_index);
        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            render_target.recreate_swapchain();
            continue;
        } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            quix_error("failed to acquire swapchain image");
        }
        sync_objects.reset_fence(current_frame);

        command_lists[current_frame]->begin_record();

        command_lists[current_frame]->begin_render_pass(render_target, pipeline, current_image_index, clear_values.data(), clear_values.size());

        vkCmdBindVertexBuffers(command_lists[current_frame]->get_cmd_buffer(), 0, vertex_buffer_array.size(), vertex_buffer_array.data(), offsets.data());

        vkCmdBindIndexBuffer(command_lists[current_frame]->get_cmd_buffer(), index_buffer.get_buffer(), 0, VK_INDEX_TYPE_UINT16);

        vkCmdBindDescriptorSets(command_lists[current_frame]->get_cmd_buffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->get_layout(), 0, 1, &descriptor_sets[current_frame], 0, nullptr);

        vkCmdDrawIndexed(command_lists[current_frame]->get_cmd_buffer(), indices.size(), 1, 0, 0, 0);

        command_lists[current_frame]->end_render_pass();

        command_lists[current_frame]->end_record();

        update_uniform();

        VK_CHECK(sync_objects.submit_frame(current_frame, command_lists[current_frame].get()), "failed to submit frame");

        result = sync_objects.present_frame(current_frame, current_image_index);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || window->get_framebuffer_resized()) {
            render_target.recreate_swapchain();
        } else if (result != VK_SUCCESS) {
            quix_error("failed to present swapchain image");
        }
        current_frame = (current_frame + 1) % FRAMES_IN_FLIGHT;
    }

    instance.wait_idle();

    return 0;
}
