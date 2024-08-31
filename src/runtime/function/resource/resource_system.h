#pragma once

#include "core/base/bitmask.hpp"
#include "core/uuid/uuid_v4.h"
#include "function/render/structs/image_data.h"
#include "function/render/structs/model.h"
#include "function/render/structs/shader.h"
#include "function/system.h"

#include <unordered_map>
#include <vector>

namespace Meow
{
    /**
     * @brief Beside loading resource from disk to memory,
     * Resource System can also handle sharing, solving dependencies of resources, reloading.
     *
     * 1. Sharing resources
     *
     * If game object A has load resource Res1, and then new a game object B which also need Res1,
     * Resource System can directly provide Res1 for B, but not load a duplicate of Res1.
     *
     * 2. Solving Dependencies
     *
     * If loading a game object need loading resource Res1, and Res1 depends on Res2, Res3,
     * Resource System will load Res2, Res3 firstly, and then load Res1.
     *
     * 3. Reloading from disk
     *
     * When engine is running, it will auto detect all resource files and reload the updated files.
     */
    class ResourceSystem final : public System
    {
    public:
        ResourceSystem();
        ~ResourceSystem();

        void Start() override;

        void Tick(float dt) override;

        bool LoadTexture(const std::string& file_path, UUIDv4::UUID& uuid);

        std::shared_ptr<ImageData> GetTexture(const UUIDv4::UUID& uuid);

        // bool LoadMaterial(const std::string& file_path, UUIDv4::UUID& uuid);

        // std::shared_ptr<Material> GetMaterial(const UUIDv4::UUID& uuid);

        bool LoadModel(std::vector<float>&&        vertices,
                       std::vector<uint32_t>&&     indices,
                       BitMask<VertexAttributeBit> attributes,
                       UUIDv4::UUID&               uuid);

        bool LoadModel(const std::string& file_path, BitMask<VertexAttributeBit> attributes, UUIDv4::UUID& uuid);

        std::shared_ptr<Model> GetModel(const UUIDv4::UUID& uuid);

    private:
        UUIDv4::UUIDGenerator<std::mt19937_64> m_uuid_generator;

        std::unordered_map<std::string, UUIDv4::UUID>                m_textures_path2id;
        std::unordered_map<UUIDv4::UUID, std::shared_ptr<ImageData>> m_textures_id2data;

        // std::unordered_map<UUIDv4::UUID, std::shared_ptr<Material>> m_materials;

        std::unordered_map<std::string, UUIDv4::UUID>            m_models_path2id;
        std::unordered_map<UUIDv4::UUID, std::shared_ptr<Model>> m_models_id2data;
    };
} // namespace Meow
