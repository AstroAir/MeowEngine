#pragma once

#include "core/base/bitmask.hpp"
#include "function/object/game_object.h"
#include "function/render/structs/buffer_data.h"
#include "function/render/structs/builtin_render_stat.h"
#include "function/render/structs/image_data.h"
#include "function/render/structs/material.h"
#include "function/render/structs/shader.h"
#include "function/render/structs/surface_data.h"
#include "function/render/structs/swapchain_data.h"
#include "function/render/structs/ubo_data.h"
#include "function/system.h"
#include "function/window/window.h"
#include "render_pass/deferred_pass.h"
#include "render_pass/forward_pass.h"
#include "render_pass/imgui_pass.h"

#include <vulkan/vulkan_raii.hpp>

#include <functional>
#include <memory>
#include <queue>
#include <vector>

namespace Meow
{
    struct PerFrameData
    {
        vk::raii::CommandPool   command_pool   = nullptr;
        vk::raii::CommandBuffer command_buffer = nullptr;

        vk::raii::Semaphore image_acquired_semaphore  = nullptr;
        vk::raii::Semaphore render_finished_semaphore = nullptr;
        vk::raii::Fence     in_flight_fence           = nullptr;

        PerFrameData() {}

        PerFrameData(std::nullptr_t) {}
    };

    class RenderSystem final : public System
    {
    public:
        RenderSystem();
        ~RenderSystem();

        void Start() override;

        void Tick(float dt) override;

        void SetResized(bool resized) { m_framebuffer_resized = resized; }

        void UploadPipelineStat(const std::string& pass_name, const std::vector<uint32_t>& stat)
        {
            m_pipeline_stat_map[pass_name] = stat;
        }

        const std::unordered_map<std::string, std::vector<uint32_t>>& GetPipelineStat() { return m_pipeline_stat_map; }

        void UploadBuiltinRenderStat(const std::string& pass_name, BuiltinRenderStat stat)
        {
            m_render_stat_map[pass_name] = stat;
        }

        const std::unordered_map<std::string, BuiltinRenderStat>& GetBuiltinRenderStat() { return m_render_stat_map; }

        std::shared_ptr<ImageData> CreateTexture(const std::string& file_path);

        std::shared_ptr<Model> CreateModel(std::vector<float>&&        vertices,
                                           std::vector<uint32_t>&&     indices,
                                           BitMask<VertexAttributeBit> attributes);

        std::shared_ptr<Model> CreateModel(const std::string& file_path, BitMask<VertexAttributeBit> attributes);

        UUIDv4::UUID m_main_camera_id;

    private:
        void CreateVulkanInstance();
#if defined(VKB_DEBUG) || defined(VKB_VALIDATION_LAYERS)
        void CreateDebugUtilsMessengerEXT();
#endif
        void CreatePhysicalDevice();
        void CreateSurface();
        void CreateLogicalDevice();
        void CreateSwapChian();
        void CreateDescriptorAllocator();
        void CreatePerFrameData();
        void CreateRenderPass();
        void InitImGui();
        void RecreateSwapChain();

        /**
         * @brief vk::raii::SwapchainKHR::acquireNextImageKHR without exceptions
         */
        std::pair<vk::Result, uint32_t> SwapchainNextImageWrapper(const vk::raii::SwapchainKHR& swapchain,
                                                                  uint64_t                      timeout,
                                                                  vk::Semaphore                 semaphore,
                                                                  vk::Fence                     fence = {});

        /**
         * @brief vk::raii::Queue::presentKHR without exceptions
         */
        vk::Result QueuePresentWrapper(const vk::raii::Queue& queue, const vk::PresentInfoKHR& present_info);

        bool m_iconified = false;

        bool                           m_is_validation_layer_found  = false;
        const std::vector<const char*> k_required_device_extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
        const uint64_t                 k_fence_timeout              = 100000000;
        const uint32_t                 k_max_frames_in_flight       = 2;

        uint32_t m_current_frame_index = 0;
        uint32_t m_current_image_index = 0;

        uint32_t m_graphics_queue_family_index = 0;
        uint32_t m_present_queue_family_index  = 0;

        bool m_framebuffer_resized = false;

        vk::raii::Context  m_vulkan_context;
        vk::raii::Instance m_vulkan_instance = nullptr;
#if defined(VKB_DEBUG) || defined(VKB_VALIDATION_LAYERS)
        vk::raii::DebugUtilsMessengerEXT m_debug_utils_messenger = nullptr;
#endif
        vk::raii::PhysicalDevice    m_gpu                  = nullptr;
        SurfaceData                 m_surface_data         = nullptr;
        vk::raii::Device            m_logical_device       = nullptr;
        vk::raii::Queue             m_graphics_queue       = nullptr;
        vk::raii::Queue             m_present_queue        = nullptr;
        SwapChainData               m_swapchain_data       = nullptr;
        DescriptorAllocatorGrowable m_descriptor_allocator = nullptr;
        DeferredPass                m_deferred_pass        = nullptr;
        ForwardPass                 m_forward_pass         = nullptr;
        ImGuiPass                   m_imgui_pass           = nullptr;
        std::vector<PerFrameData>   m_per_frame_data;

        RenderPass*                                            m_render_pass_ptr = nullptr;
        std::unordered_map<std::string, std::vector<uint32_t>> m_pipeline_stat_map;
        std::unordered_map<std::string, BuiltinRenderStat>     m_render_stat_map;

        // TODO: Dynamic descriptor pool?
        vk::raii::DescriptorPool m_imgui_descriptor_pool = nullptr;
    };
} // namespace Meow
