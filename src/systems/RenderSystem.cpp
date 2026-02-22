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
    auto& transforms = world.GetStore<Components::TransformComponent>();
    auto& renders    = world.GetStore<Components::RenderComponent>();
    auto& sprites    = world.GetStore<Components::SpriteComponent>();
    auto& cameras    = world.GetStore<Components::CameraComponent>();

    f32 cameraX = 0.0f;
    f32 cameraY = 0.0f;

    for (auto& [entity, camera] : cameras.GetAll()) {
        cameraX = camera.x;
        cameraY = camera.y;
        break;
    }

    for (auto& [entity, render] : renders.GetAll()) {
        if (!transforms.Has(entity)) continue;

        auto& transform = transforms.Get(entity);

        SDL_FRect dst {
            transform.x - cameraX,
            transform.y - cameraY,
            static_cast<float>(render.width),
            static_cast<float>(render.height)
        };

        SDL_SetRenderDrawColor(m_renderer, render.r, render.g, render.b, 255);
        SDL_RenderFillRect(m_renderer, &dst);
    }

    for (auto& [entity, sprite] : sprites.GetAll()) {
        if (!transforms.Has(entity)) continue;
        if (!sprite.texture)         continue;

        auto& transform = transforms.Get(entity);

        SDL_FRect dst {
            transform.x - cameraX,
            transform.y - cameraY,
            0.0f, 0.0f
        };

        if (sprite.srcRect) {
            dst.w = sprite.srcRect->w;
            dst.h = sprite.srcRect->h;
        } else {
            float w, h;
            SDL_GetTextureSize(sprite.texture, &w, &h);
            dst.w = w;
            dst.h = h;
        }

        SDL_RenderTexture(m_renderer, sprite.texture, sprite.srcRect, &dst);
    }
}

} // namespace Systems