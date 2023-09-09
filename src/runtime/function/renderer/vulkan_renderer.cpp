#include "vulkan_renderer.h"
#include "core/log/log.h"

#include <map>

namespace Meow
{
    /**
     * @brief Create Vulkan context.
     */
    void VulkanRenderer::CreateContext()
    {
        m_vulkan_context = std::make_shared<vk::raii::Context>();

#ifdef MEOW_DEBUG
        vk::Meow::LogVulkanAPIVersion(m_vulkan_context->enumerateInstanceVersion());
#endif
    }

    /**
     * @brief Create the Vulkan instance.
     * <p> If build in Debug mode, create DebugUtilsMessengerEXT at the same time.
     *
     * @param required_instance_extensions The required Vulkan instance extensions.
     * @param required_validation_layers The required Vulkan validation layers
     */
    void VulkanRenderer::CreateInstance(std::vector<const char*> const& required_instance_extensions_base,
                                        std::vector<const char*> const& required_validation_layers_base)
    {
        // prepare for create vk::InstanceCreateInfo

        std::vector<vk::ExtensionProperties> available_instance_extensions =
            m_vulkan_context->enumerateInstanceExtensionProperties();

        std::vector<const char*> required_instance_extensions =
            vk::Meow::GetRequiredInstanceExtensions(required_instance_extensions_base);

        if (!vk::Meow::ValidateExtensions(required_instance_extensions, available_instance_extensions))
        {
            throw std::runtime_error("Required instance extensions are missing.");
        }

        std::vector<vk::LayerProperties> supported_validation_layers =
            m_vulkan_context->enumerateInstanceLayerProperties();

        std::vector<const char*> required_validation_layers(required_validation_layers_base);

#ifdef VKB_VALIDATION_LAYERS
        // Determine the optimal validation layers to enable that are necessary for useful debugging
        std::vector<const char*> optimal_validation_layers =
            vk::Meow::GetOptimalValidationLayers(supported_validation_layers);
        required_validation_layers.insert(
            required_validation_layers.end(), optimal_validation_layers.begin(), optimal_validation_layers.end());
#endif

        if (vk::Meow::ValidateLayers(required_validation_layers, supported_validation_layers))
        {
            RUNTIME_INFO("Enabled Validation Layers:");
            for (const auto& layer : required_validation_layers)
            {
                RUNTIME_INFO("	\t{}", layer);
            }
        }
        else
        {
            throw std::runtime_error("Required validation layers are missing.");
        }

        uint32_t api_version = m_vulkan_context->enumerateInstanceVersion();

        vk::ApplicationInfo app("Meow Engine Vulkan Renderer", {}, "Meow Engine", {}, api_version);

        vk::InstanceCreateInfo instance_info({}, &app, required_validation_layers, required_instance_extensions);
#if defined(VKB_DEBUG) || defined(VKB_VALIDATION_LAYERS)
        // VkDebugUtilsMessengerEXT only covers stuff from its creation to its destruction.
        // vkCreateInstance and vkDestroyInstance are covered by the special pNext variant
        // because at that point the VkDebugUtilsMessengerEXT object cannot even exist yet\anymore
        vk::DebugUtilsMessengerCreateInfoEXT debug_utils_create_info = vk::Meow::MakeDebugUtilsMessengerCreateInfoEXT();
        instance_info.pNext                                          = &debug_utils_create_info;
#endif

#if (defined(VKB_ENABLE_PORTABILITY))
        instance_info.flags |= vk::InstanceCreateFlagBits::eEnumeratePortabilityKHR;
#endif

        m_vulkan_instance = std::make_shared<vk::raii::Instance>(*m_vulkan_context, instance_info);

#if defined(VKB_DEBUG) || defined(VKB_VALIDATION_LAYERS)
        m_debug_utils_messenger =
            std::make_shared<vk::raii::DebugUtilsMessengerEXT>(*m_vulkan_instance, debug_utils_create_info);
#endif

#if defined(VK_USE_PLATFORM_DISPLAY_KHR)
        // we need some additional initializing for this platform!
        if (volkInitialize())
        {
            throw std::runtime_error("Failed to initialize volk.");
        }
        volkLoadInstance(instance);
#endif
    }

    /**
     * @brief Rank all physical devices and choose the one with highest score.
     */
    void VulkanRenderer::CreatePhysicalDevice()
    {
        vk::raii::PhysicalDevices gpus(*m_vulkan_instance);

        // Maps to hold devices and sort by rank.
        std::multimap<uint32_t, vk::raii::PhysicalDevice> ranked_devices;
        auto                                              where = ranked_devices.end();

        // Iterates through all devices and rate their suitability.
        for (const auto& gpu : gpus)
            where =
                ranked_devices.insert(where, {vk::Meow::ScorePhysicalDevice(gpu, k_required_device_extensions), gpu});

        // Checks to make sure the best candidate scored higher than 0  rbegin points to last element of ranked
        // devices(highest rated), first is its rating.
        if (ranked_devices.rbegin()->first > 0)
            m_gpu = std::make_shared<vk::raii::PhysicalDevice>(ranked_devices.rbegin()->second);
    }

    /**
     * @brief Create Surface and delete old surface if old surface exists
     */
    void VulkanRenderer::CreateSurface()
    {
        VkSurfaceKHR surface = m_window.lock()->CreateSurface((**m_vulkan_instance));
        if (!surface)
        {
            throw std::runtime_error("Failed to create window surface.");
        }

        // delete old surface if old surface exists
        m_surface.reset(new vk::raii::SurfaceKHR(*m_vulkan_instance, surface));
    }

    /**
     * @brief Create the Vulkan physical device and logical device.
     *
     * @param context A Vulkan context with an instance already set up.
     * @param required_device_extensions The required Vulkan device extensions.
     */
    void VulkanRenderer::CreateLogicalDeviceAndQueue()
    {
        std::vector<vk::ExtensionProperties> device_extensions = m_gpu->enumerateDeviceExtensionProperties();

        if (!vk::Meow::ValidateExtensions(k_required_device_extensions, device_extensions))
        {
            throw std::runtime_error("Required device extensions are missing, will try without.");
        }

        auto indexs                   = vk::Meow::FindGraphicsAndPresentQueueFamilyIndex(*m_gpu, *m_surface);
        m_graphics_queue_family_index = indexs.first;
        m_present_queue_family_index  = indexs.second;

        // Create a device with one queue
        float                     queue_priority = 1.0f;
        vk::DeviceQueueCreateInfo queue_info({}, m_graphics_queue_family_index, 1, &queue_priority);
        vk::DeviceCreateInfo      device_info({}, queue_info, {}, k_required_device_extensions);

        m_logical_device = std::make_shared<vk::raii::Device>(*m_gpu, device_info);

#if defined(VK_USE_PLATFORM_DISPLAY_KHR)
        volkLoadDevice(**m_logical_device);
#endif

        m_graphics_queue = std::make_shared<vk::raii::Queue>(*m_logical_device, m_graphics_queue_family_index, 0);
        m_present_queue  = std::make_shared<vk::raii::Queue>(*m_logical_device, m_present_queue_family_index, 0);
    }

    /**
     * @brief Create command pool and command buffer.
     * @todo Support multi thread.
     */
    void VulkanRenderer::CreateCommandBuffer()
    {
        vk::CommandPoolCreateInfo command_pool_create_info({}, m_graphics_queue_family_index);
        m_command_pool = std::make_shared<vk::raii::CommandPool>(*m_logical_device, command_pool_create_info);

        vk::CommandBufferAllocateInfo command_buffer_allocate_info(
            **m_command_pool, vk::CommandBufferLevel::ePrimary, 1);
        m_command_buffer = std::make_shared<vk::raii::CommandBuffer>(
            std::move(vk::raii::CommandBuffers(*m_logical_device, command_buffer_allocate_info).front()));
    }

    void VulkanRenderer::CreateSwapChain()
    {
        auto         size = m_window.lock()->GetSize();
        vk::Extent2D extent(size.x, size.y);

        m_swapchain_data = std::make_shared<vk::Meow::SwapChainData>(*m_gpu,
                                                                     *m_logical_device,
                                                                     *m_surface,
                                                                     extent,
                                                                     vk::ImageUsageFlagBits::eColorAttachment |
                                                                         vk::ImageUsageFlagBits::eTransferSrc,
                                                                     nullptr,
                                                                     m_graphics_queue_family_index,
                                                                     m_present_queue_family_index);
    }

    VulkanRenderer::VulkanRenderer(std::shared_ptr<Window> window) : m_window(window)
    {
        CreateContext();
        CreateInstance({VK_KHR_SURFACE_EXTENSION_NAME}, {});
        CreatePhysicalDevice();
        CreateSurface();
        CreateLogicalDeviceAndQueue();
        CreateCommandBuffer();
        CreateSwapChain();
    }

    VulkanRenderer::~VulkanRenderer() {}
} // namespace Meow