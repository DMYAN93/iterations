#pragma once
#include "ecs/System.h"

namespace Components { struct TilemapComponent; }

namespace Systems {

// Resolves collisions between entities with CollisionComponent and
// the tilemap. Runs after MovementSystem so it corrects position
// before CameraSystem reads it. Checks X and Y axes independently
// so entities can slide along walls rather than stopping dead.
class CollisionSystem : public ECS::System {
public:
    void Update(ECS::World& world, float deltaTime) override;

private:
    bool OverlapsNonWalkable(
        float x, float y, float width, float height,
        const Components::TilemapComponent& tilemap) const;
};

} // namespace Systems