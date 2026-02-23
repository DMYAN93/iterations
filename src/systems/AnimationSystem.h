#pragma once
#include "ecs/System.h"

namespace Systems {

// Drives SpriteComponent::srcRect each tick based on AnimationComponent state.
// Register as an update system, before RenderSystem.
class AnimationSystem : public ECS::System {
public:
    void Update(ECS::World& world, float deltaTime) override;
};

} // namespace Systems