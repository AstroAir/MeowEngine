#pragma once

#include "function/systems/render/structs/material.h"
#include "function/systems/render/structs/texture.h"
#include "function/systems/system.h"

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

        void Update(float frame_time);

        // std::unordered_map<std::string, Texture>  m_textures;
        std::unordered_map<std::string, Material> m_materials;
    };
} // namespace Meow
