#ifndef _QUIX_COMMAND_LIST_CPP
#define _QUIX_COMMAND_LIST_CPP

#include "quix_command_list.hpp"

#include <utility>
#include <vulkan/vulkan_core.h>

#include "quix_device.hpp"
#include "quix_pipeline.hpp"
#include "quix_render_target.hpp"
#include "quix_swapchain.hpp"

namespace quix {

sync::sync(weakref<device> p_device, weakref<swapchain> p_swapchain)
    : m_device(std::move(p_device))
    , m_swapchain(std::move(p_swapchain))
    , m_frames_in_flight(m_swapchain->get_frames_in_flight())
{
    create_sync_objects();
}

sync::~sync()
{
    destroy_sync_objects();
}

void sync::wait_for_fence(const int frame)
{
    vkWaitForFences(m_device->get_logical_device(), 1, &m_fences[frame], VK_TRUE, UINT64_MAX);
}

void sync::reset_fence(const int frame)
{
    vkResetFences(m_device->get_logical_device(), 1, &m_fences[frame]);
}

VkResult sync::acquire_next_image(const int frame, uint32_t* image_index)
{
    vkWaitForFences(m_device->get_logical_device(), 1, &m_fences[frame], VK_TRUE, UINT64_MAX);
    return vkAcquireNextImageKHR(m_device->get_logical_device(), m_swapchain->get_swapchain(), UINT64_MAX, m_available_semaphores[frame], VK_NULL_HANDLE, image_index);
}

VkResult sync::submit_frame(const int frame, command_list* command)
{
    VkSubmitInfo submitInfo {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = { m_available_semaphores[frame] };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = command->get_cmd_buffer_ref();

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
    quix_assert(m_sync_buffer != nullptr, "failed to allocate memory for synchronization objects");

    m_fences = (VkFence*)m_sync_buffer;
    m_available_semaphores = (VkSemaphore*)((char*)m_sync_buffer + sizeof(VkFence) * m_frames_in_flight);
    m_finished_semaphores = (VkSemaphore*)((char*)m_sync_buffer + sizeof(VkFence) * m_frames_in_flight + sizeof(VkSemaphore) * m_frames_in_flight);

    VkSemaphoreCreateInfo semaphoreInfo {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo {};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (int i = 0; i < m_frames_in_flight; i++) {
        VK_CHECK(vkCreateSemaphore(m_device->get_logical_device(), &semaphoreInfo, nullptr, &m_available_semaphores[i]), "failed to create semaphore");
        VK_CHECK(vkCreateSemaphore(m_device->get_logical_device(), &semaphoreInfo, nullptr, &m_finished_semaphores[i]), "failed to create semaphore");
        VK_CHECK(vkCreateFence(m_device->get_logical_device(), &fenceInfo, nullptr, &m_fences[i]), "failed to create fence");
    }
}

void sync::destroy_sync_objects()
{
    for (int i = 0; i < m_frames_in_flight; i++) {
        vkDestroySemaphore(m_device->get_logical_device(), m_available_semaphores[i], nullptr);
        vkDestroySemaphore(m_device->get_logical_device(), m_finished_semaphores[i], nullptr);
        vkDestroyFence(m_device->get_logical_device(), m_fences[i], nullptr);
    }
    free(m_sync_buffer);
}

command_list::command_list(weakref<device> p_device, VkCommandBuffer buffer)
    : m_device(std::move(p_device))
    , buffer(buffer)
{
}

void command_list::begin_record(VkCommandBufferUsageFlags flags)
{
    VkCommandBufferBeginInfo begin_info {};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    /*VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT: The command buffer will be rerecorded right after executing it once.
      VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT: This is a secondary command buffer that will be entirely within a single render pass.
      VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT: The command buffer can be resubmitted while it is also already pending execution. */
    begin_info.flags = flags;
    begin_info.pInheritanceInfo = nullptr; // for secondary command buffers

    VK_CHECK(vkBeginCommandBuffer(buffer, &begin_info), "failed to begin command buffer record");
}

void command_list::end_record()
{
    VK_CHECK(vkEndCommandBuffer(buffer), "failed to record command buffer!");
}

void command_list::begin_render_pass(const render_target& r_target, const std::shared_ptr<graphics::pipeline>& p_pipeline, uint32_t image_index, VkClearValue* clear_value, uint32_t clear_value_count)
{
    VkRenderPassBeginInfo render_pass_begin_info {};
    render_pass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    render_pass_begin_info.renderPass = r_target.get_render_pass();
    render_pass_begin_info.framebuffer = r_target.get_framebuffer(image_index);
    VkRect2D render_area {};
    render_area.offset = { 0, 0 };
    render_area.extent = r_target.get_extent();
    render_pass_begin_info.renderArea = render_area;
    render_pass_begin_info.clearValueCount = clear_value_count;
    render_pass_begin_info.pClearValues = clear_value;

    vkCmdBeginRenderPass(buffer, &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, p_pipeline->get_pipeline());

    VkViewport viewport {};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(r_target.get_extent().width);
    viewport.height = static_cast<float>(r_target.get_extent().height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0;

    vkCmdSetViewport(buffer, 0, 1, &viewport);

    VkRect2D scissor {};
    scissor.offset = { 0, 0 };
    scissor.extent = r_target.get_extent();

    vkCmdSetScissor(buffer, 0, 1, &scissor);
}

void command_list::end_render_pass()
{
    vkCmdEndRenderPass(buffer);
}

void command_list::copy_buffer_to_buffer(VkBuffer src_buffer, VkDeviceSize src_offset, VkBuffer dst_buffer, VkDeviceSize dst_offset, VkDeviceSize size)
{
    VkBufferCopy copyRegion {};
    copyRegion.srcOffset = src_offset;
    copyRegion.dstOffset = dst_offset;
    copyRegion.size = size;

    vkCmdCopyBuffer(buffer, src_buffer, dst_buffer, 1, &copyRegion);
}

command_pool::command_pool(weakref<device> p_device, VkCommandPool pool)
    : m_device(std::move(p_device))
    , pool(pool)
{
}

command_pool::~command_pool()
{
    m_device->return_command_pool(pool);
}

NODISCARD allocated_unique_ptr<command_list> command_pool::create_command_list(VkCommandBufferLevel level)
{
    VkCommandBufferAllocateInfo alloc_info {};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.commandPool = pool;
    alloc_info.level = level;
    alloc_info.commandBufferCount = 1;

    VkCommandBuffer buffer = VK_NULL_HANDLE;
    vkAllocateCommandBuffers(m_device->get_logical_device(), &alloc_info, &buffer);

    return allocate_unique<command_list>(&m_allocator, m_device, buffer);
}

} // namespace quix

#endif // _QUIX_COMMAND_LIST_CPP