#pragma once
#include "ecs/System.h"
#include "core/InputManager.h"

namespace Systems {

class MovementSystem : public ECS::System {
public:
    MovementSystem(Core::InputManager& input, float speed);
    void Update(ECS::World& world, float deltaTime) override;

private:
    Core::InputManager& m_input;
    float               m_speed;
};

} // namespace Systems