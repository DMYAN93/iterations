#include "systems/RenderSystem.h"
#include "ecs/World.h"
#include "components/TransformComponent.h"
#include "components/RenderComponent.h"
#include "components/SpriteComponent.h"
#include "components/CameraComponent.h"

namespace Systems {

RenderSystem::RenderSystem(SDL_Renderer* renderer)
    : m_renderer(renderer) {
}

void RenderSystem::Update(ECS::World& world, float deltaTime) {
    (void)deltaTime;
    auto& renders    = world.GetStore<Components::RenderComponent>();
    auto& sprites    = world.GetStore<Components::SpriteComponent>();
    auto& cameras    = world.GetStore<Components::CameraComponent>();

    f32 cameraX = 0.0f;
    f32 cameraY = 0.0f;
    const auto cameraEntities = world.View<Components::CameraComponent>();
    static bool loggedCameraCountIssue = false;
    if (cameraEntities.size() == 1) {
        const auto& camera = cameras.Get(cameraEntities.front());
        cameraX = camera.x;
        cameraY = camera.y;
        loggedCameraCountIssue = false;
    } else if (!loggedCameraCountIssue) {
        SDL_Log("RenderSystem: expected exactly 1 camera, found %zu. Using world-origin fallback camera.", cameraEntities.size());
        loggedCameraCountIssue = true;
    }

    for (ECS::Entity entity : world.View<Components::RenderComponent, Components::TransformComponent>()) {
        auto& render = renders.Get(entity);
        auto& transform = world.GetComponent<Components::TransformComponent>(entity);

        SDL_FRect dst {
            transform.x - (render.width  / 2.0f) - cameraX,
            transform.y - (render.height / 2.0f) - cameraY,
            static_cast<float>(render.width),
            static_cast<float>(render.height)
        };

        SDL_SetRenderDrawColor(m_renderer, render.r, render.g, render.b, 255);
        SDL_RenderFillRect(m_renderer, &dst);
    }

    for (ECS::Entity entity : world.View<Components::SpriteComponent, Components::TransformComponent>()) {
        auto& sprite = sprites.Get(entity);
        if (!sprite.texture) continue;
        auto& transform = world.GetComponent<Components::TransformComponent>(entity);

        float w, h;
        if (sprite.srcRect.has_value()) {
            w = sprite.srcRect->w;
            h = sprite.srcRect->h;
        } else {
            SDL_GetTextureSize(sprite.texture, &w, &h);
        }

        SDL_FRect dst {
            transform.x - (w / 2.0f) + sprite.offsetX - cameraX,
            transform.y - (h / 2.0f) + sprite.offsetY - cameraY,
            w, h
        };

        const SDL_FRect* src = sprite.srcRect.has_value() ? &sprite.srcRect.value() : nullptr;
        SDL_RenderTexture(m_renderer, sprite.texture, src, &dst);
    }
}

} // namespace Systems
