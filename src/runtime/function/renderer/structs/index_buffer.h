#pragma once

#include "core/base/non_copyable.h"
#include "function/renderer/utils/vulkan_hpp_utils.hpp"

#include <vulkan/vulkan_raii.hpp>

namespace Meow
{
    struct IndexBuffer : NonCopyable
    {
        vk::Meow::BufferData buffer_data;
        uint32_t             count = 0;
        vk::IndexType        type  = vk::IndexType::eUint16;

        template<typename T>
        IndexBuffer(vk::raii::PhysicalDevice const& physical_device,
                    vk::raii::Device const&         device,
                    vk::DeviceSize                  size,
                    vk::MemoryPropertyFlags         property_flags = vk::MemoryPropertyFlagBits::eHostVisible |
                                                             vk::MemoryPropertyFlagBits::eHostCoherent,
                    T const*      p_data = nullptr,
                    uint32_t      _count = 0,
                    vk::IndexType _type  = vk::IndexType::eUint16)
            : buffer_data(physical_device, device, size, vk::BufferUsageFlagBits::eIndexBuffer, property_flags)
            , count(_count)
            , type(_type)
        {
            vk::Meow::CopyToDevice(buffer_data.device_memory, p_data, count);
        }

        void BindDraw(const vk::raii::CommandBuffer& cmd_buffer) const;
    };
} // namespace Meow