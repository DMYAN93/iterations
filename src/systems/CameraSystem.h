#pragma once
#include "ecs/System.h"

namespace Systems {

class CameraSystem : public ECS::System {
public:
    void Update(ECS::World& world, float deltaTime) override;
};

} // namespace Systems