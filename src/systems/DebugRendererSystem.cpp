#include "systems/DebugRendererSystem.h"
#include "ecs/World.h"
#include "components/TransformComponent.h"
#include "components/CollisionComponent.h"
#include "components/SpriteComponent.h"
#include "components/TilemapComponent.h"
#include "components/CameraComponent.h"

namespace Systems {

DebugRenderSystem::DebugRenderSystem(SDL_Renderer* renderer, Core::DebugRenderer& debugRenderer, const Core::SettingsManager& settings)
    : m_renderer(renderer), m_debugRenderer(debugRenderer), m_settings(settings) {
}

void DebugRenderSystem::Update(ECS::World& world, float deltaTime) {
    auto& transforms = world.GetStore<Components::TransformComponent>();
    auto& cameras    = world.GetStore<Components::CameraComponent>();

    f32 cameraX = 0.0f;
    f32 cameraY = 0.0f;
    for (auto& [entity, camera] : cameras.GetAll()) {
        cameraX = camera.x;
        cameraY = camera.y;
        break;
    }

    // Collision hitboxes
    if (m_settings.IsDebugEnabled(m_settings.debug.showCollision)) {
        auto& collisions = world.GetStore<Components::CollisionComponent>();
        for (auto& [entity, collision] : collisions.GetAll()) {
            if (!transforms.Has(entity)) continue;
            auto& transform = transforms.Get(entity);

            m_debugRenderer.DrawRect(
                transform.x - (collision.width  / 2.0f) + collision.offsetX - cameraX,
                transform.y - (collision.height / 2.0f) + collision.offsetY - cameraY,
                collision.width,
                collision.height,
                {0, 255, 0, 255}
            );
        }
    }

    // Sprite bounds
    if (m_settings.IsDebugEnabled(m_settings.debug.spriteBounds)) {
        auto& sprites = world.GetStore<Components::SpriteComponent>();
        for (auto& [entity, sprite] : sprites.GetAll()) {
            if (!transforms.Has(entity)) continue;
            if (!sprite.texture)         continue;
            auto& transform = transforms.Get(entity);

            float w, h;
            if (sprite.srcRect.has_value()) {
                w = sprite.srcRect->w;
                h = sprite.srcRect->h;
            } else {
                SDL_GetTextureSize(sprite.texture, &w, &h);
            }

            m_debugRenderer.DrawRect(
                transform.x - (w / 2.0f) + sprite.offsetX - cameraX,
                transform.y - (h / 2.0f) + sprite.offsetY - cameraY,
                w, h,
                {255, 0, 255, 255}
            );
        }
    }

    // Tile grid and walkability
    if (m_settings.IsDebugEnabled(m_settings.debug.showTileGrid) ||
        m_settings.IsDebugEnabled(m_settings.debug.showWalkability)) {

        auto& tilemaps = world.GetStore<Components::TilemapComponent>();
        for (auto& [entity, tilemap] : tilemaps.GetAll()) {
            for (i32 row = 0; row < static_cast<i32>(tilemap.grid.size()); ++row) {
                for (i32 col = 0; col < static_cast<i32>(tilemap.grid[row].size()); ++col) {
                    const Components::Tile& tile = tilemap.grid[row][col];

                    SDL_FRect dst {
                        static_cast<float>(col * tilemap.tileWidth)  - cameraX,
                        static_cast<float>(row * tilemap.tileHeight) - cameraY,
                        static_cast<float>(tilemap.tileWidth),
                        static_cast<float>(tilemap.tileHeight)
                    };

                    if (m_settings.IsDebugEnabled(m_settings.debug.showWalkability)) {
                        int index = row * static_cast<i32>(tilemap.grid[row].size()) + col;
                        if (tilemap.blockedTiles.count(index)) {
                            m_debugRenderer.DrawFilledRect(dst.x, dst.y, dst.w, dst.h, {255, 0, 0, 80});
                        }
                    }

                    if (m_settings.IsDebugEnabled(m_settings.debug.showTileGrid)) {
                        m_debugRenderer.DrawRect(dst.x, dst.y, dst.w, dst.h, {255, 255, 255, 40});
                    }
                }
            }
        }
    }
}

} // namespace Systems