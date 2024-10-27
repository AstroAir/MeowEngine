#pragma once

#include "core/base/macro.h"
#include "core/math/bounding_box.h"
#include "core/math/frustum.h"
#include "core/reflect/macros.h"
#include "function/components/transform/transform_3d_component.hpp"
#include "function/object/game_object.h"
#include <glm/glm.hpp>

namespace Meow
{
    enum class CameraMode : unsigned char
    {
        ThirdPerson,
        FirstPerson,
        Free,
        Invalid
    };

    class [[reflectable_class()]] LIBRARY_API Camera3DComponent : public Component
    {
    public:
        [[reflectable_field()]]
        float field_of_view = glm::radians(45.0f);

        [[reflectable_field()]]
        float aspect_ratio = 16.0 / 9.0;

        [[reflectable_field()]]
        float near_plane = 0.1f;

        [[reflectable_field()]]
        float far_plane = 1000.0f;

        [[reflectable_field()]]
        CameraMode camera_mode = CameraMode::Invalid;

        void Start() override;

        void Tick(float dt) override;

        bool FrustumCulling(std::shared_ptr<GameObject> gameobject);

        bool CheckVisibility(BoundingBox* bounding);

    private:
        void TickFreeCamera(float dt);

        std::weak_ptr<Transform3DComponent> m_transform;
        Frustum                             m_frustum;
    };
} // namespace Meow