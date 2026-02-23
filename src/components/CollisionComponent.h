#pragma once
#include "core/Types.h"

namespace Components {

// Axis-aligned bounding box for collision, relative to the entity's
// transform center. offsetX/offsetY shift the hitbox from center —
// useful when the logical collision area doesn't match the sprite center.
// Width and height define the full hitbox size, not half-extents.
struct CollisionComponent {
    float offsetX = 0.0f;
    float offsetY = 0.0f;
    float width   = 16.0f;
    float height  = 16.0f;
};

} // namespace Components