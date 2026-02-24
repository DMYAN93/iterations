#include "systems/DebugRendererSystem.h"
#include "ecs/World.h"
#include "components/TransformComponent.h"
#include "components/CollisionComponent.h"
#include "components/SpriteComponent.h"
#include "components/TilemapComponent.h"
#include "components/CameraComponent.h"
#include <algorithm>
#include <cmath>

namespace Systems {

DebugRenderSystem::DebugRenderSystem(SDL_Renderer* renderer, Core::DebugRenderer& debugRenderer, const Core::SettingsManager& settings)
    : m_renderer(renderer), m_debugRenderer(debugRenderer), m_settings(settings) {
}

void DebugRenderSystem::Update(ECS::World& world, float deltaTime) {
    auto& transforms = world.GetStore<Components::TransformComponent>();
    auto& cameras    = world.GetStore<Components::CameraComponent>();

    f32 cameraX = 0.0f;
    f32 cameraY = 0.0f;
    i32 cameraViewportWidth = 0;
    i32 cameraViewportHeight = 0;
    bool hasActiveCamera = false;
    for (auto& [entity, camera] : cameras.GetAll()) {
        (void)entity;
        cameraX = camera.x;
        cameraY = camera.y;
        cameraViewportWidth = camera.viewportWidth;
        cameraViewportHeight = camera.viewportHeight;
        hasActiveCamera = true;
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
            (void)entity;

            const i32 rowCount = static_cast<i32>(tilemap.grid.size());
            if (rowCount <= 0) continue;

            const i32 columnCount = static_cast<i32>(tilemap.grid[0].size());
            if (columnCount <= 0) continue;

            i32 minVisibleCol = 0;
            i32 maxVisibleCol = columnCount - 1;
            i32 minVisibleRow = 0;
            i32 maxVisibleRow = rowCount - 1;

            if (hasActiveCamera && tilemap.tileWidth > 0 && tilemap.tileHeight > 0) {
                const float worldLeft = cameraX;
                const float worldTop = cameraY;
                const float worldRight = cameraX + static_cast<float>(cameraViewportWidth);
                const float worldBottom = cameraY + static_cast<float>(cameraViewportHeight);

                minVisibleCol = static_cast<i32>(std::floor(worldLeft / static_cast<float>(tilemap.tileWidth)));
                maxVisibleCol = static_cast<i32>(std::floor((worldRight - 1.0f) / static_cast<float>(tilemap.tileWidth)));
                minVisibleRow = static_cast<i32>(std::floor(worldTop / static_cast<float>(tilemap.tileHeight)));
                maxVisibleRow = static_cast<i32>(std::floor((worldBottom - 1.0f) / static_cast<float>(tilemap.tileHeight)));

                minVisibleCol = std::max(0, minVisibleCol);
                maxVisibleCol = std::min(columnCount - 1, maxVisibleCol);
                minVisibleRow = std::max(0, minVisibleRow);
                maxVisibleRow = std::min(rowCount - 1, maxVisibleRow);

                if (minVisibleCol > maxVisibleCol || minVisibleRow > maxVisibleRow) {
                    continue;
                }
            }

            for (i32 row = minVisibleRow; row <= maxVisibleRow; ++row) {
                for (i32 col = minVisibleCol; col <= maxVisibleCol; ++col) {
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
