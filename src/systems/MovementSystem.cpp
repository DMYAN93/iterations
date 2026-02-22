#include "systems/MovementSystem.h"
#include "ecs/World.h"
#include "components/PlayerComponent.h"
#include "components/TransformComponent.h"
#include <SDL3/SDL.h>

namespace Systems {

MovementSystem::MovementSystem(Core::InputManager& input, float speed)
    : m_input(input), m_speed(speed) {
}

void MovementSystem::Update(ECS::World& world, float deltaTime) {
    auto& transforms = world.GetStore<Components::TransformComponent>();
    auto& players    = world.GetStore<Components::PlayerComponent>();

    for (auto& [entity, _] : players.GetAll()) {
        if (!transforms.Has(entity)) continue;

        auto& transform = transforms.Get(entity);

        if (m_input.IsKeyDown(SDL_SCANCODE_W)) transform.y -= m_speed * deltaTime;
        if (m_input.IsKeyDown(SDL_SCANCODE_S)) transform.y += m_speed * deltaTime;
        if (m_input.IsKeyDown(SDL_SCANCODE_A)) transform.x -= m_speed * deltaTime;
        if (m_input.IsKeyDown(SDL_SCANCODE_D)) transform.x += m_speed * deltaTime;
    }
}

} // namespace Systems