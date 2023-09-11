#pragma once

#include "core/base/non_copyable.h"
#include "function/renderer/utils/vulkan_hpp_utils.hpp"
#include "function/renderer/window.h"

#include <volk.h>
#include <vulkan/vulkan_raii.hpp>

#include "core/base/macro.h"

#include <memory>

namespace Meow
{
    class VulkanRenderer : NonCopyable
    {
    public:
        VulkanRenderer(std::shared_ptr<Window> window);

        ~VulkanRenderer();

        void Update();

    private:
        void CreateContext();
        void CreateInstance(std::vector<const char*> const& required_instance_extensions,
                            std::vector<const char*> const& required_validation_layers);
        void CreatePhysicalDevice();
        void CreateSurface();
        void CreateLogicalDeviceAndQueue();
        void CreateCommandBuffer();
        void CreateSwapChain();
        void CreateDepthBuffer();
        void CreateUniformBuffer();
        void CreatePipelineLayout();
        void CreateDescriptorSet();
        void CreateRenderPass();
        void CreateShaders();
        void CreateFrameBuffer();
        void CreateVertexBuffer();
        void CreatePipeline();
        void CreateSyncObjects();
        bool StartRenderpass(uint32_t& image_index);
        void EndRenderpass(uint32_t& image_index);

        const std::vector<const char*> k_required_device_extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

        std::weak_ptr<Window>                               m_window;
        std::shared_ptr<vk::raii::Context>                  m_vulkan_context;
        std::shared_ptr<vk::raii::Instance>                 m_vulkan_instance;
        std::shared_ptr<vk::raii::PhysicalDevice>           m_gpu;
        std::shared_ptr<vk::Meow::SurfaceData>              m_surface_data;
        uint32_t                                            m_graphics_queue_family_index;
        uint32_t                                            m_present_queue_family_index;
        std::shared_ptr<vk::raii::Device>                   m_logical_device;
        std::shared_ptr<vk::raii::Queue>                    m_graphics_queue;
        std::shared_ptr<vk::raii::Queue>                    m_present_queue;
        std::shared_ptr<vk::raii::CommandPool>              m_command_pool;
        std::shared_ptr<vk::raii::CommandBuffer>            m_command_buffer;
        std::shared_ptr<vk::Meow::SwapChainData>            m_swapchain_data;
        std::shared_ptr<vk::Meow::DepthBufferData>          m_depth_buffer_data;
        std::shared_ptr<vk::Meow::BufferData>               m_uniform_buffer_data;
        std::shared_ptr<vk::raii::DescriptorSetLayout>      m_descriptor_set_layout;
        std::shared_ptr<vk::raii::PipelineLayout>           m_pipeline_layout;
        std::shared_ptr<vk::raii::DescriptorPool>           m_descriptor_pool;
        std::shared_ptr<vk::raii::DescriptorSet>            m_descriptor_set;
        std::shared_ptr<vk::raii::RenderPass>               m_render_pass;
        std::shared_ptr<vk::raii::ShaderModule>             m_vertex_shader_module;
        std::shared_ptr<vk::raii::ShaderModule>             m_fragment_shader_module;
        std::shared_ptr<std::vector<vk::raii::Framebuffer>> m_framebuffers;
        std::shared_ptr<vk::Meow::BufferData>               m_vertex_buffer_data;
        std::shared_ptr<vk::raii::Pipeline>                 m_graphics_pipeline;
        std::shared_ptr<vk::raii::Semaphore>                m_image_acquired_semaphore;
        std::shared_ptr<vk::raii::Semaphore>                m_render_finished_semaphore;
        std::shared_ptr<vk::raii::Fence>                    m_draw_fence;

#if defined(VKB_DEBUG) || defined(VKB_VALIDATION_LAYERS)
        std::shared_ptr<vk::raii::DebugUtilsMessengerEXT> m_debug_utils_messenger;
#endif
    };
} // namespace Meow
