#ifndef _QUIX_COMMAND_LIST_CPP
#define _QUIX_COMMAND_LIST_CPP

#include "quix_commands.hpp"

#include "quix_device.hpp"
#include "quix_pipeline.hpp"
#include "quix_render_target.hpp"
#include "quix_resource.hpp"
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
    VkBufferCopy copy_region {};
    copy_region.srcOffset = src_offset;
    copy_region.dstOffset = dst_offset;
    copy_region.size = size;

    vkCmdCopyBuffer(buffer, src_buffer, dst_buffer, 1, &copy_region);
}

void command_list::copy_buffer_to_image(VkBuffer src_buffer, VkDeviceSize buffer_offset, image_handle* dst_image, VkOffset3D image_offset, VkImageAspectFlags aspect_mask)
{
    VkBufferImageCopy copy_region {};
    copy_region.bufferOffset = buffer_offset;
    copy_region.bufferRowLength = 0;
    copy_region.bufferImageHeight = 0;
    
    copy_region.imageOffset = image_offset;
    copy_region.imageExtent = dst_image->m_extent;

    copy_region.imageSubresource.aspectMask = aspect_mask;
    copy_region.imageSubresource.baseArrayLayer = 0;
    copy_region.imageSubresource.layerCount = dst_image->m_array_layers;
    copy_region.imageSubresource.mipLevel = dst_image->m_mip_levels-1;

    vkCmdCopyBufferToImage(
        buffer, src_buffer,
        dst_image->get_image(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1, &copy_region);
}

void command_list::copy_image_to_image(image_handle* src, VkOffset3D src_offset, image_handle* dst, VkOffset3D dst_offset, VkImageAspectFlags aspect_mask)
{
    VkImageCopy copy_region {};
    copy_region.srcOffset = src_offset;
    copy_region.srcSubresource.layerCount = src->m_array_layers;
    copy_region.srcSubresource.baseArrayLayer = 0;
    copy_region.srcSubresource.mipLevel = src->m_mip_levels;
    copy_region.srcSubresource.aspectMask = aspect_mask;

    copy_region.dstOffset = dst_offset;
    copy_region.dstSubresource.layerCount = dst->m_array_layers;
    copy_region.dstSubresource.baseArrayLayer = 0;
    copy_region.dstSubresource.mipLevel = dst->m_mip_levels;
    copy_region.dstSubresource.aspectMask = aspect_mask;
    copy_region.extent = src->m_extent;

    vkCmdCopyImage(buffer, src->get_image(),
        VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, dst->get_image(),
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copy_region);
}

void command_list::image_barrier(image_handle* image, image_barrier_info* barrier_info, VkImageAspectFlags aspect_mask)
{
    VkImageMemoryBarrier memory_barrier_info {};
    memory_barrier_info.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    memory_barrier_info.image = image->get_image();
    memory_barrier_info.oldLayout = barrier_info->old_layout;
    memory_barrier_info.newLayout = barrier_info->new_layout;
    memory_barrier_info.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    memory_barrier_info.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

    memory_barrier_info.subresourceRange.aspectMask = aspect_mask;
    memory_barrier_info.subresourceRange.baseMipLevel = 0;
    memory_barrier_info.subresourceRange.levelCount = image->m_mip_levels;
    memory_barrier_info.subresourceRange.baseArrayLayer = 0;
    memory_barrier_info.subresourceRange.layerCount = image->m_array_layers;

    memory_barrier_info.srcAccessMask = barrier_info->src_access_mask;
    memory_barrier_info.dstAccessMask = barrier_info->dst_access_mask;

    vkCmdPipelineBarrier(
        buffer,
        barrier_info->src_stage, barrier_info->dst_stage,
        0,
        0, nullptr,
        0, nullptr,
        1, &memory_barrier_info);
}

void command_list::submit(VkFence fence)
{
    VkSubmitInfo submitInfo {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &buffer;

    VK_CHECK(vkQueueSubmit(m_device->get_graphics_queue(), 1, &submitInfo, fence), "failed to submit command buffer");
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
