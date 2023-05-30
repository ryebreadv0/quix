#ifndef _QUIX_COMMAND_LIST_CPP
#define _QUIX_COMMAND_LIST_CPP

#include "quix_command_list.hpp"

#include "quix_device.hpp"
#include "quix_pipeline.hpp"
#include "quix_render_target.hpp"
#include "quix_swapchain.hpp"

namespace quix {

sync::sync(std::shared_ptr<device> s_device, std::shared_ptr<swapchain> s_swapchain)
    : m_device(s_device)
    , m_swapchain(s_swapchain)
    , m_frames_in_flight(s_swapchain->get_frames_in_flight())
{
    create_sync_objects();
}

sync::~sync()
{
    destroy_sync_objects();
}

void sync::sync_frame(const int frame)
{
    vkWaitForFences(m_device->get_logical_device(), 1, &m_fences[frame], VK_TRUE, UINT64_MAX);
    vkResetFences(m_device->get_logical_device(), 1, &m_fences[frame]);
}

VkResult sync::acquire_next_image(const int frame, uint32_t* image_index)
{
    vkWaitForFences(m_device->get_logical_device(), 1, &m_fences[frame], VK_TRUE, UINT64_MAX);
    VkResult result = vkAcquireNextImageKHR(m_device->get_logical_device(), m_swapchain->get_swapchain(), UINT64_MAX, m_available_semaphores[frame], VK_NULL_HANDLE, image_index);
    vkResetFences(m_device->get_logical_device(), 1, &m_fences[frame]);
    return result;
}

VkResult sync::submit_frame(const int frame, std::shared_ptr<command_list> command)
{
    VkSubmitInfo submitInfo {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = { m_available_semaphores[frame] };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = command->get_buffer_ref();

    VkSemaphore signalSemaphores[] = { m_finished_semaphores[frame] };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    return vkQueueSubmit(m_device->get_graphics_queue(), 1, &submitInfo, m_fences[frame]);
}

VkResult sync::present_frame(const int frame, const uint32_t image_index)
{
    VkPresentInfoKHR presentInfo {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    VkSemaphore signalSemaphores[] = { m_finished_semaphores[frame] };
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = { m_swapchain->get_swapchain() };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &image_index;
    presentInfo.pResults = nullptr; // Optional

    return vkQueuePresentKHR(m_device->get_present_queue(), &presentInfo);
}

void sync::create_sync_objects()
{
    m_sync_buffer = malloc(sizeof(VkSemaphore) * (m_frames_in_flight * 2) + sizeof(VkFence) * m_frames_in_flight);
    if (m_sync_buffer == nullptr) {
        throw std::runtime_error("failed to allocate memory for synchronization objects!");
    }
    m_fences = (VkFence*)m_sync_buffer;
    m_available_semaphores = (VkSemaphore*)((char*)m_sync_buffer + sizeof(VkFence) * m_frames_in_flight);
    m_finished_semaphores = (VkSemaphore*)((char*)m_sync_buffer + sizeof(VkFence) * m_frames_in_flight + sizeof(VkSemaphore) * m_frames_in_flight);

    VkSemaphoreCreateInfo semaphoreInfo {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo {};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < m_frames_in_flight; i++) {
        if (vkCreateSemaphore(m_device->get_logical_device(), &semaphoreInfo, nullptr, &m_available_semaphores[i]) != VK_SUCCESS || vkCreateSemaphore(m_device->get_logical_device(), &semaphoreInfo, nullptr, &m_finished_semaphores[i]) != VK_SUCCESS || vkCreateFence(m_device->get_logical_device(), &fenceInfo, nullptr, &m_fences[i]) != VK_SUCCESS) {

            throw std::runtime_error("failed to create synchronization objects for a frame!");
        }
    }
}

void sync::destroy_sync_objects()
{
    for (size_t i = 0; i < m_frames_in_flight; i++) {
        vkDestroySemaphore(m_device->get_logical_device(), m_available_semaphores[i], nullptr);
        vkDestroySemaphore(m_device->get_logical_device(), m_finished_semaphores[i], nullptr);
        vkDestroyFence(m_device->get_logical_device(), m_fences[i], nullptr);
    }
    free(m_sync_buffer);
}

command_list::command_list(std::shared_ptr<device> s_device, VkCommandBuffer buffer)
    : m_device(s_device)
    , buffer(buffer)
{
}

void command_list::begin_record(VkCommandBufferUsageFlags flags)
{
    VkCommandBufferBeginInfo begin_info {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        /*VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT: The command buffer will be rerecorded right after executing it once.
          VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT: This is a secondary command buffer that will be entirely within a single render pass.
          VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT: The command buffer can be resubmitted while it is also already pending execution. */
        .flags = 0,
        .pInheritanceInfo = nullptr // for secondary command buffers
    };

    if (vkBeginCommandBuffer(buffer, &begin_info) != VK_SUCCESS) {
        spdlog::error("failed to begin recording command buffer!");
        throw std::runtime_error("failed to begin recording command buffer!");
    }
}

void command_list::end_record()
{
    if (vkEndCommandBuffer(buffer) != VK_SUCCESS) {
        spdlog::error("failed to record command buffer!");
        throw std::runtime_error("failed to record command buffer!");
    }
}

void command_list::begin_render_pass(std::shared_ptr<render_target> target, graphics::pipeline& pipeline, uint32_t image_index, VkClearValue* clear_value, uint32_t clear_value_count)
{
    VkRenderPassBeginInfo render_pass_begin_info {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .renderPass = target->get_render_pass(),
        .framebuffer = target->get_framebuffer(image_index),
        .renderArea = {
            .offset = { 0, 0 },
            .extent = target->get_extent() },
        .clearValueCount = clear_value_count,
        .pClearValues = clear_value
    };

    vkCmdBeginRenderPass(buffer, &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.get_pipeline());

    VkViewport viewport {
        .x = 0.0f,
        .y = 0.0f,
        .width = static_cast<float>(target->get_extent().width),
        .height = static_cast<float>(target->get_extent().height),
        .minDepth = 0.0f,
        .maxDepth = 1.0f
    };

    vkCmdSetViewport(buffer, 0, 1, &viewport);

    VkRect2D scissor {
        .offset = { 0, 0 },
        .extent = target->get_extent()
    };

    vkCmdSetScissor(buffer, 0, 1, &scissor);
}

void command_list::end_render_pass()
{
    vkCmdEndRenderPass(buffer);
}

command_pool::command_pool(std::shared_ptr<device> s_device, VkCommandPool pool)
    : m_device(s_device)
    , pool(pool)
{
}

command_pool::~command_pool()
{
    m_device->return_command_pool(pool);
}

NODISCARD std::shared_ptr<command_list> command_pool::create_command_list() const
{
    VkCommandBufferAllocateInfo alloc_info {};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.commandPool = pool;
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandBufferCount = 1;

    VkCommandBuffer buffer;
    vkAllocateCommandBuffers(m_device->get_logical_device(), &alloc_info, &buffer);

    return std::make_shared<command_list>(m_device, buffer);
}

} // namespace quix

#endif // _QUIX_COMMAND_LIST_CPP