#pragma once
#include <SDL3/SDL.h>
#include <vector>
#include <unordered_map>
#include <string>

namespace Components {

struct AnimationClip {
    std::vector<SDL_FRect> frames;
    float                  frameDuration = 0.1f; // seconds per frame
    bool                   looping       = true;
};

// Drives SpriteComponent::srcRect each tick.
// Add clips by name, set currentClip to switch animation.
// AnimationSystem will not switch clips mid-play unless you change currentClip.
struct AnimationComponent {
    std::unordered_map<std::string, AnimationClip> clips;
    std::string currentClip;
    int         currentFrame   = 0;
    float       elapsedTime    = 0.0f;
};

} // namespace Components