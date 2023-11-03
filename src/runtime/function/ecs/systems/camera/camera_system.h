#pragma once

#include "function/ecs/components/3d/camera/camera_3d_component.h"
#include "function/ecs/components/3d/transform/transform_3d_component.h"
#include "function/ecs/system.h"

namespace Meow
{
    class CameraSystem : System
    {
    public:
        void Update(float frame_time);

    private:
        void UpdateFreeCamera(Transform3DComponent& transform_component,
                              Camera3DComponent&    camera_component,
                              float                 frame_time);
    };
} // namespace Meow