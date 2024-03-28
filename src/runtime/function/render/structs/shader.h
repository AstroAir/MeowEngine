#pragma once

#include "buffer_data.h"
#include "descriptor_allocator_growable.h"
#include "texture_data.hpp"
#include "ubo_data.h"
#include "vertex_attribute.h"

#include <spirv_glsl.hpp>
#include <vulkan/vulkan_raii.hpp>

#include <cstdint>
#include <unordered_map>

namespace Meow
{
    struct VertexAttributeMeta
    {
        VertexAttribute attribute;
        int32_t         location;
    };

    struct BufferMeta
    {
        uint32_t             set            = 0;
        uint32_t             binding        = 0;
        uint32_t             bufferSize     = 0;
        vk::DescriptorType   descriptorType = vk::DescriptorType::eUniformBuffer;
        vk::ShaderStageFlags stageFlags     = {};
    };

    struct ImageMeta
    {
        uint32_t             set            = 0;
        uint32_t             binding        = 0;
        vk::DescriptorType   descriptorType = vk::DescriptorType::eCombinedImageSampler;
        vk::ShaderStageFlags stageFlags     = {};
    };

    class DescriptorSetLayoutMeta
    {
    private:
        typedef std::vector<vk::DescriptorSetLayoutBinding> BindingsArray;

    public:
        DescriptorSetLayoutMeta() {}

        ~DescriptorSetLayoutMeta() {}

    public:
        int32_t       set = -1;
        BindingsArray bindings;
    };

    class DescriptorSetLayoutsMeta
    {
    public:
        struct BindingMeta
        {
            int32_t set;
            int32_t binding;
        };

        DescriptorSetLayoutsMeta() {}

        ~DescriptorSetLayoutsMeta() {}

        vk::DescriptorType GetDescriptorType(int32_t set, int32_t binding)
        {
            for (int32_t i = 0; i < set_layout_metas.size(); ++i)
            {
                if (set_layout_metas[i].set == set)
                {
                    for (int32_t j = 0; j < set_layout_metas[i].bindings.size(); ++j)
                    {
                        if (set_layout_metas[i].bindings[j].binding == binding)
                        {
                            return set_layout_metas[i].bindings[j].descriptorType;
                        }
                    }
                }
            }

            // There is not vk::DescriptorType correspond to VK_DESCRIPTOR_TYPE_MAX_ENUM?
            return vk::DescriptorType::eMutableEXT;
        }

        void
        AddDescriptorSetLayoutBinding(const std::string& varName, int32_t set, vk::DescriptorSetLayoutBinding binding)
        {
            DescriptorSetLayoutMeta* setLayout = nullptr;

            // find existing set layout
            // this supports multiple set
            for (int32_t i = 0; i < set_layout_metas.size(); ++i)
            {
                if (set_layout_metas[i].set == set)
                {
                    setLayout = &(set_layout_metas[i]);
                    break;
                }
            }

            // If there is not set layout, new one
            if (setLayout == nullptr)
            {
                set_layout_metas.push_back({});
                setLayout = &(set_layout_metas[set_layout_metas.size() - 1]);
            }

            for (int32_t i = 0; i < setLayout->bindings.size(); ++i)
            {
                vk::DescriptorSetLayoutBinding& setBinding = setLayout->bindings[i];
                if (setBinding.binding == binding.binding && setBinding.descriptorType == binding.descriptorType)
                {
                    setBinding.stageFlags = setBinding.stageFlags | binding.stageFlags;
                    return;
                }
            }

            setLayout->set = set;
            setLayout->bindings.push_back(binding);

            // save mapping from parameter name to set and binding
            BindingMeta paramInfo = {};
            paramInfo.set         = set;
            paramInfo.binding     = binding.binding;
            binding_meta_map.insert(std::make_pair(varName, paramInfo));
        }

    public:
        std::unordered_map<std::string, BindingMeta> binding_meta_map;
        std::vector<DescriptorSetLayoutMeta>         set_layout_metas;
    };

    struct Shader
    {
        typedef std::vector<vk::VertexInputBindingDescription>   InputBindingsVector;
        typedef std::vector<vk::VertexInputAttributeDescription> InputAttributesVector;

        bool use_dynamic_uniform_buffer = false;

        DescriptorSetLayoutsMeta set_layouts_meta;

        std::vector<VertexAttributeMeta>            vertex_attribute_metas;
        std::unordered_map<std::string, BufferMeta> buffer_meta_map;
        std::unordered_map<std::string, ImageMeta>  image_meta_map;

        std::vector<VertexAttribute> per_vertex_attributes;
        std::vector<VertexAttribute> instances_attributes;

        InputBindingsVector   input_bindings;
        InputAttributesVector input_attributes;

        // stored to create descriptor pool
        std::vector<vk::DescriptorSetLayout> descriptor_set_layouts;

        vk::raii::PipelineLayout pipeline_layout = nullptr;

        vk::raii::DescriptorSets descriptor_sets = nullptr;

        // vector storing temp info and temp WriteDescriptorSet

        std::vector<vk::DescriptorBufferInfo> descriptor_buffer_infos;
        std::vector<vk::DescriptorImageInfo>  descriptor_image_infos;
        std::vector<vk::WriteDescriptorSet>   write_descriptor_sets;

        // TODO: create pipeline in material
        vk::raii::Pipeline graphics_pipeline = nullptr;

        Shader() {}

        Shader(vk::raii::PhysicalDevice const&    gpu,
               vk::raii::Device const&            logical_device,
               DescriptorAllocatorGrowable& descriptor_allocator,
               vk::raii::RenderPass const&        render_pass,
               std::string                        vert_shader_file_path,
               std::string                        frag_shader_file_path,
               std::string                        geom_shader_file_path = "",
               std::string                        comp_shader_file_path = "",
               std::string                        tesc_shader_file_path = "",
               std::string                        tese_shader_file_path = "");

        void PushBufferWrite(const std::string&          name,
                             vk::raii::Buffer const&     buffer,
                             vk::raii::BufferView const* raii_buffer_view = nullptr);

        void PushImageWrite(const std::string& name, TextureData& texture_data);

        void UpdateDescriptorSets(vk::raii::Device const& logical_device);

        void Bind(vk::raii::CommandBuffer const& command_buffer);

    private:
        bool CreateShaderModuleAndGetMeta(
            vk::raii::Device const&                         logical_device,
            vk::raii::ShaderModule&                         shader_module,
            const std::string&                              shader_file_path,
            vk::ShaderStageFlagBits                         stage,
            std::vector<vk::PipelineShaderStageCreateInfo>& pipeline_shader_stage_create_infos);

        void GetAttachmentsMeta(spirv_cross::Compiler&        compiler,
                                spirv_cross::ShaderResources& resources,
                                vk::ShaderStageFlags          stageFlags);

        void GetUniformBuffersMeta(spirv_cross::Compiler&        compiler,
                                   spirv_cross::ShaderResources& resources,
                                   vk::ShaderStageFlags          stageFlags);

        void GetTexturesMeta(spirv_cross::Compiler&        compiler,
                             spirv_cross::ShaderResources& resources,
                             vk::ShaderStageFlags          stageFlags);

        void GetInputMeta(spirv_cross::Compiler&        compiler,
                          spirv_cross::ShaderResources& resources,
                          vk::ShaderStageFlags          stageFlags);

        void GetStorageBuffersMeta(spirv_cross::Compiler&        compiler,
                                   spirv_cross::ShaderResources& resources,
                                   vk::ShaderStageFlags          stageFlags);

        void GetStorageImagesMeta(spirv_cross::Compiler&        compiler,
                                  spirv_cross::ShaderResources& resources,
                                  vk::ShaderStageFlags          stageFlags);

        void GenerateInputInfo();

        void GenerateLayout(vk::raii::Device const& raii_logical_device);

        /**
         * @brief Allocate multiple descriptor sets
         * Each set correspond to different set number in glsl, for example:
         *
         * layout (set = 0, binding = 0) ...
         * layout (set = 1, binding = 0) ...
         *
         * It will result in two descriptor sets allocated.
         *
         * @param logical_device logical device
         * @param descriptor_allocator descriptor allocator
         */
        void AllocateDescriptorSet(vk::raii::Device const&      logical_device,
                                   DescriptorAllocatorGrowable& descriptor_allocator);
    };

} // namespace Meow
