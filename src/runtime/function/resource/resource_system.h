#pragma once

#include "core/base/bitmask.hpp"
#include "function/render/structs/image_data.h"
#include "function/render/structs/model.h"
#include "function/render/structs/shader.h"
#include "function/system.h"

#include <unordered_map>

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

        std::shared_ptr<ImageData> LoadTexture(const std::string& file_path);

        std::shared_ptr<ImageData> GetTexture(const std::string& file_path);

        // bool LoadMaterial(const std::string& file_path);

        // std::shared_ptr<Material> GetMaterial(const std::string& file_path);

        std::shared_ptr<Model> LoadModel(const std::string& file_path, BitMask<VertexAttributeBit> attributes);

        std::shared_ptr<Model> GetModel(const std::string& file_path);

    private:
        /**
         * @brief Relative path - ImageData
         */
        std::unordered_map<std::string, std::shared_ptr<ImageData>> m_textures;

        // /**
        //  * @brief Relative path - Material
        //  */
        // std::unordered_map<std::string, std::shared_ptr<Material>> m_materials;

        std::unordered_map<std::string, std::shared_ptr<Model>> m_models;
    };
} // namespace Meow
