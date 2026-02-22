#pragma once
#include "core/Types.h"

namespace Components {

// Marks an entity as the active camera.
// x, y represent the top-left corner of the viewport in world space.
struct CameraComponent {
    f32 x, y;
    i32 viewportWidth, viewportHeight;
};

} // namespace Components