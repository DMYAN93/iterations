#pragma once
#include <SDL3/SDL.h>
#include <optional>

namespace Components {

// offsetX/offsetY are relative to the entity's transform center.
// If this entity also has an AnimationComponent, AnimationSystem will
// write the current frame rect into srcRect each tick. Otherwise
// srcRect can be set manually or left empty to draw the full texture.
// Layer/depth ordering is a TODO — will be driven by asset data when
// the asset pipeline is built.
struct SpriteComponent {
    SDL_Texture*             texture   = nullptr;
    std::optional<SDL_FRect> srcRect;
    float                    offsetX   = 0.0f;
    float                    offsetY   = 0.0f;
    bool                     debugDraw = false;
};

} // namespace Components