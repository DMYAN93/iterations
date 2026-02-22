#pragma once
#include <SDL3/SDL.h>

namespace Components {

// Holds a reference to a loaded texture and the source rectangle within it.
// srcRect defines which part of the texture to draw — useful for spritesheets.
// If srcRect is null, the entire texture is drawn.
struct SpriteComponent {
    SDL_Texture*  texture = nullptr;
    SDL_FRect*    srcRect = nullptr;
};

} // namespace Components