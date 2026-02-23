#pragma once
#include "core/Types.h"

namespace Components {

// prevX/prevY are written by MovementSystem at the start of each move,
// before the new position is applied. CollisionSystem uses them to snap
// back to a known good position on collision.
struct TransformComponent {
    f32 x     = 0.0f;
    f32 y     = 0.0f;
    f32 prevX = 0.0f;
    f32 prevY = 0.0f;
};

} // namespace Components