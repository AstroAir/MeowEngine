#include "render_pass.h"

#include "pch.h"

#include "function/global/runtime_global_context.h"

namespace Meow
{
    RenderPass::RenderPass(vk::raii::Device const& device) {}

    void RenderPass::Start(vk::raii::CommandBuffer const& command_buffer,
                           Meow::SurfaceData const&       surface_data,
                           uint32_t                       current_image_index)
    {
        FUNCTION_TIMER();

        vk::RenderPassBeginInfo render_pass_begin_info(*render_pass,
                                                       *framebuffers[current_image_index],
                                                       vk::Rect2D(vk::Offset2D(0, 0), surface_data.extent),
                                                       clear_values);
        command_buffer.beginRenderPass(render_pass_begin_info, vk::SubpassContents::eInline);
    }

    void RenderPass::End(vk::raii::CommandBuffer const& command_buffer)
    {
        FUNCTION_TIMER();

        command_buffer.endRenderPass();
    }

    void RenderPass::AfterPresent() {}

    void swap(RenderPass& lhs, RenderPass& rhs)
    {
        using std::swap;

        swap(lhs.render_pass, rhs.render_pass);
        swap(lhs.framebuffers, rhs.framebuffers);
        swap(lhs.clear_values, rhs.clear_values);
        swap(lhs.input_vertex_attributes, rhs.input_vertex_attributes);

        swap(lhs.m_pass_name, rhs.m_pass_name);
        swap(lhs.m_query_enabled, rhs.m_query_enabled);
        swap(lhs.query_pool, rhs.query_pool);

        swap(lhs.m_depth_format, rhs.m_depth_format);
        swap(lhs.m_sample_count, rhs.m_sample_count);
        swap(lhs.m_depth_attachment, rhs.m_depth_attachment);
    }
} // namespace Meow