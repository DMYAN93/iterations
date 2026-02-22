#include "systems/RenderSystem.h"
#include "components/TransformComponent.h"
#include "components/RenderComponent.h"
#include "ecs/World.h"

namespace Systems {

RenderSystem::RenderSystem(SDL_Renderer* renderer)
    : m_renderer(renderer) {
}

void RenderSystem::Update(ECS::World& world, float deltaTime) {
    auto& transforms = world.GetStore<Components::TransformComponent>();
    auto& renders = world.GetStore<Components::RenderComponent>();

    for (auto& [entity, render] : renders.GetAll()) {
        if (!transforms.Has(entity)) continue;

        auto& transform = transforms.Get(entity);

        SDL_FRect rect {
            transform.x,
            transform.y,
            static_cast<float>(render.width),
            static_cast<float>(render.height)
        };

        SDL_SetRenderDrawColor(m_renderer, render.r, render.g, render.b, 255);
        SDL_RenderFillRect(m_renderer, &rect);
    }
}

} // namespace Systems